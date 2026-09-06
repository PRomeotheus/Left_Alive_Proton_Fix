#!/usr/bin/env bash
#
# Install / remove lashim.dll in a Proton (Wine) prefix.
#
#   ./install.sh                    install into the AppID 714370 prefix
#   ./install.sh --appid 123456     install into another game's prefix
#   ./install.sh --prefix /path/pfx install into an explicit Wine prefix
#   ./install.sh --status           show what CLSID_SampleGrabber points at
#   ./install.sh --uninstall        restore the original registration
#   ./install.sh --force            proceed even if the prefix looks busy
#
# Scope: this touches exactly one Wine prefix - the one named by --appid or
# --prefix, defaulting to 714370 (LEFT ALIVE). Proton gives every game its
# own prefix under steamapps/compatdata/<appid>, so nothing outside that
# directory is read or written and no other game is affected.
#
# Registry edits are made directly to system.reg, so no Wine binary is
# needed. STEAM AND THE GAME MUST BE CLOSED: wineserver rewrites the registry
# files when the last process in the prefix exits and would clobber the edit.
#
# The DLL can also register itself from inside the prefix with
#   regsvr32 lashim.dll
# which does the same thing through the registry API.

set -euo pipefail
cd "$(dirname "$0")"

SRC_DLL="$PWD/lashim.dll"
CLSID="c1f400a0-3f08-11d3-9f0b-006008039e37"   # CLSID_SampleGrabber
APPID=714370
PFX=""
MODE=install
FORCE=0

while [[ $# -gt 0 ]]; do
    case "$1" in
        --appid)     APPID="$2"; shift 2 ;;
        --appid=*)   APPID="${1#*=}"; shift ;;
        --prefix)    PFX="$2"; shift 2 ;;
        --prefix=*)  PFX="${1#*=}"; shift ;;
        --status)    MODE=status; shift ;;
        --uninstall) MODE=uninstall; shift ;;
        --force)     FORCE=1; shift ;;
        -h|--help)   sed -n '2,26p' "$0"; exit 0 ;;
        *) echo "unknown argument: $1" >&2; exit 1 ;;
    esac
done

# ---- locate the prefix ----------------------------------------------------
if [[ -z "$PFX" ]]; then
    for base in "$HOME/.local/share/Steam/steamapps" \
                "$HOME/.steam/steam/steamapps" \
                "$HOME/.steam/root/steamapps" \
                /run/media/*/steamapps \
                /run/media/*/*/steamapps; do
        if [[ -d "$base/compatdata/$APPID/pfx" ]]; then
            PFX="$base/compatdata/$APPID/pfx"; break
        fi
    done
fi

if [[ -z "$PFX" || ! -d "$PFX" ]]; then
    echo "error: no prefix found for AppID $APPID" >&2
    echo "       run the game once so Proton creates it, or pass --prefix" >&2
    exit 1
fi

REG="$PFX/system.reg"
SYS32="$PFX/drive_c/windows/system32"
SIDECAR="$PFX/.lashim-previous-inprocserver32"

[[ -f "$REG" ]] || { echo "error: $REG missing - is this a Wine prefix?" >&2; exit 1; }

echo "prefix      : $PFX"

# ---- safety: nothing may be running in this prefix ------------------------
if [[ "$MODE" != "status" ]]; then
    if pgrep -f "compatdata/$APPID" >/dev/null 2>&1 || pgrep -x wineserver >/dev/null 2>&1; then
        if [[ $FORCE -eq 0 ]]; then
            echo "error: a wineserver / prefix process is running." >&2
            echo "       Close the game and Steam (Steam -> Exit), wait ~10s, re-run." >&2
            echo "       wineserver rewrites system.reg on exit and would undo this." >&2
            echo "       Use --force to override." >&2
            exit 1
        fi
        echo "warning     : prefix looks busy, continuing because of --force" >&2
    fi
fi

if [[ "$MODE" == "install" && ! -f "$SRC_DLL" ]]; then
    echo "error: lashim.dll not found next to this script - run ./build.sh first" >&2
    exit 1
fi

export LASHIM_REG="$REG" LASHIM_UREG="$PFX/user.reg" LASHIM_MODE="$MODE" \
       LASHIM_CLSID="$CLSID" LASHIM_SIDECAR="$SIDECAR"

python3 - <<'PY'
import os, re, shutil, sys, time

reg     = os.environ["LASHIM_REG"]
ureg    = os.environ["LASHIM_UREG"]
mode    = os.environ["LASHIM_MODE"]
clsid   = os.environ["LASHIM_CLSID"]
sidecar = os.environ["LASHIM_SIDECAR"]

OURS   = r'C:\\windows\\system32\\lashim.dll'   # .reg escaping: literal \\
QEDIT  = r'C:\\windows\\system32\\qedit.dll'

# Wine stores this key with an UPPERCASE GUID; registry keys are
# case-insensitive, so match that way rather than assuming either case.
KEY_RE = re.compile(
    r'^\[Software\\\\Classes\\\\CLSID\\\\\{' + re.escape(clsid) +
    r'\}\\\\InprocServer32\][^\n]*$',
    re.IGNORECASE | re.MULTILINE)
VAL_RE = re.compile(r'^@="([^"]*)"', re.MULTILINE)

def read(p):
    with open(p, 'r', encoding='utf-8', errors='surrogateescape') as f:
        return f.read()

def write(p, t):
    with open(p, 'w', encoding='utf-8', errors='surrogateescape') as f:
        f.write(t)

def bounds(text, m):
    """(start, end) of the .reg stanza whose header match is m."""
    s = m.start()
    e = text.find('\n[', s)
    return s, (len(text) if e == -1 else e + 1)

text = read(reg)
m = KEY_RE.search(text)

# ---------------------------------------------------------------- status ---
if mode == 'status':
    if not m:
        print('registration: (no InprocServer32 for CLSID_SampleGrabber)')
    else:
        s, e = bounds(text, m)
        print('registration:')
        for line in text[s:e].rstrip('\n').split('\n'):
            print('   ' + line)
        v = VAL_RE.search(text[s:e])
        cur = v.group(1) if v else '(none)'
        print('active      : ' + ('lashim' if 'lashim' in cur.lower() else 'stock qedit'))
    if os.path.exists(sidecar):
        print('saved orig  : ' + read(sidecar).strip())
    # legacy HKCU entry from earlier installs, which Wine ignores anyway
    try:
        if KEY_RE.search(read(ureg)):
            print('note        : a stale HKCU entry exists in user.reg (harmless, '
                  'removed on next install)')
    except OSError:
        pass
    sys.exit(0)

backup = reg + '.lashim-backup'
if not os.path.exists(backup):
    shutil.copy2(reg, backup)
    print('backup      : ' + backup)
else:
    print('backup      : ' + backup + ' (kept from an earlier run)')

# ------------------------------------------------------------- uninstall ---
if mode == 'uninstall':
    if not m:
        print('nothing to remove.')
        sys.exit(0)
    s, e = bounds(text, m)
    blk = text[s:e]
    v = VAL_RE.search(blk)
    orig = read(sidecar).strip() if os.path.exists(sidecar) else QEDIT
    if v:
        blk = blk[:v.start(1)] + orig + blk[v.end(1):]
        write(reg, text[:s] + blk + text[e:])
        print('restored    : InprocServer32 -> ' + orig)
    if os.path.exists(sidecar):
        os.remove(sidecar)
    sys.exit(0)

# --------------------------------------------------------------- install ---
if m:
    s, e = bounds(text, m)
    blk = text[s:e]
    v = VAL_RE.search(blk)
    cur = v.group(1) if v else ''
    if 'lashim' in cur.lower():
        print('registration: already points at lashim.dll (nothing to do)')
    else:
        if cur and not os.path.exists(sidecar):
            write(sidecar, cur + '\n')
            print('saved orig  : ' + cur)
        if v:
            blk = blk[:v.start(1)] + OURS + blk[v.end(1):]
        else:
            blk = blk.rstrip('\n') + '\n@="' + OURS + '"\n'
        if '"ThreadingModel"' not in blk:
            blk = blk.rstrip('\n') + '\n"ThreadingModel"="Both"\n'
        text = text[:s] + blk + text[e:]
        write(reg, text)
        print('rewrote     : CLSID_SampleGrabber -> lashim.dll')
else:
    stanza = ('\n[Software\\\\Classes\\\\CLSID\\\\{' + clsid.upper() +
              '}\\\\InprocServer32] ' + str(int(time.time())) + '\n'
              '@="' + OURS + '"\n"ThreadingModel"="Both"\n')
    write(reg, text.rstrip('\n') + '\n' + stanza)
    print('added       : CLSID_SampleGrabber -> lashim.dll')

# Remove any stale HKCU stanza from the pre-HKLM days. Wine does not consult
# HKCU\Software\Classes for this CLSID, but a leftover pointing at qedit.dll
# is confusing when reading the prefix later.
try:
    ut = read(ureg)
    um = KEY_RE.search(ut)
    if um:
        s, e = bounds(ut, um)
        write(ureg, ut[:s].rstrip('\n') + '\n' + ut[e:])
        print('cleaned     : stale HKCU entry removed from user.reg')
except OSError:
    pass
PY

if [[ "$MODE" == "install" ]]; then
    install -Dm644 "$SRC_DLL" "$SYS32/lashim.dll"
    echo "installed   : $SYS32/lashim.dll"
    echo "md5         : $(md5sum "$SRC_DLL" | cut -d' ' -f1)"
    echo
    echo "Optional logging, in the Steam launch options ahead of %command%:"
    echo "  PROTON_LOG=1 WINEDEBUG=+timestamp,+tid,+seh,+debugstr %command%"
    echo "Shim log: $PFX/drive_c/lashim.log"
elif [[ "$MODE" == "uninstall" ]]; then
    rm -f "$SYS32/lashim.dll"
    echo "removed     : $SYS32/lashim.dll"
fi
