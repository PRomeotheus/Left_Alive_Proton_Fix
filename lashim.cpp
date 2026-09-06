#define WIN32_LEAN_AND_MEAN
#define COBJMACROS
#include <windows.h>
#include <objbase.h>
#include <dshow.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

/* GUIDs
Declared locally with unique names so this file does not depend on
qedit.h / dmodshow.h / libstrmiids, which vary between toolchains.  */

#define DEFG(name, l, w1, w2, b0,b1,b2,b3,b4,b5,b6,b7) \
    static const GUID name = { l, w1, w2, { b0,b1,b2,b3,b4,b5,b6,b7 } };

DEFG(G_IUnknown,            0x00000000,0x0000,0x0000, 0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46)
DEFG(G_IClassFactory,       0x00000001,0x0000,0x0000, 0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46)
DEFG(G_IMarshal,            0x00000003,0x0000,0x0000, 0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46)
DEFG(G_IPersist,            0x0000010c,0x0000,0x0000, 0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46)
DEFG(G_IPersistStream,      0x00000109,0x0000,0x0000, 0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46)

DEFG(G_IBaseFilter,         0x56a86895,0x0ad4,0x11ce, 0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70)
DEFG(G_IMediaFilter,        0x56a86899,0x0ad4,0x11ce, 0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70)
DEFG(G_IMediaPosition,      0x56a868b2,0x0ad4,0x11ce, 0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70)
DEFG(G_IBasicAudio,         0x56a868b3,0x0ad4,0x11ce, 0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70)
DEFG(G_IMediaEventSink,     0x56a868a2,0x0ad4,0x11ce, 0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70)
DEFG(G_IVideoWindow,        0x56a868b4,0x0ad4,0x11ce, 0xb0,0x3a,0x00,0x20,0xaf,0x0b,0xa7,0x70)
DEFG(G_IMediaSeeking,       0x36b73880,0xc2c8,0x11cf, 0x8b,0x46,0x00,0x80,0x5f,0x6c,0xef,0x60)
DEFG(G_ISpecifyPropPages,   0xb196b28b,0xbab4,0x101a, 0xb6,0x9c,0x00,0xaa,0x00,0x34,0x1d,0x07)
DEFG(G_IAMFilterMiscFlags,  0x2dd74950,0xa890,0x11d1, 0xab,0xe8,0x00,0xa0,0xc9,0x05,0xf3,0x75)
DEFG(G_IPropertyBag,        0x55272a00,0x42cb,0x11ce, 0x81,0x35,0x00,0xaa,0x00,0x4b,0xb8,0x51)
DEFG(G_IPersistPropertyBag, 0x37d84f60,0x42cb,0x11ce, 0x81,0x35,0x00,0xaa,0x00,0x4b,0xb8,0x51)
DEFG(G_IServiceProvider,    0x6d5140c1,0x7436,0x11ce, 0x80,0x34,0x00,0xaa,0x00,0x60,0x09,0xfa)

/* qedit */
DEFG(G_CLSID_SampleGrabber, 0xc1f400a0,0x3f08,0x11d3, 0x9f,0x0b,0x00,0x60,0x08,0x03,0x9e,0x37)
DEFG(G_CLSID_NullRenderer,  0xc1f400a4,0x3f08,0x11d3, 0x9f,0x0b,0x00,0x60,0x08,0x03,0x9e,0x37)
DEFG(G_ISampleGrabber,      0x6b652fff,0x11fe,0x4fce, 0x92,0xad,0x02,0x66,0xb5,0xd7,0xc7,0x8f)
DEFG(G_ISampleGrabberCB,    0x0579154a,0x2b53,0x4994, 0xb0,0xd0,0xe7,0x73,0x14,0x8e,0xff,0x85)
DEFG(G_IDMOWrapperFilter,   0x52d6f586,0x9f0f,0x4824, 0x8f,0xc8,0xe3,0x2c,0xa0,0x49,0x30,0xc2)
DEFG(G_CLSID_DMOWrapper,    0x94297043,0xbd82,0x4dfd, 0xb0,0xde,0x81,0x77,0x73,0x9c,0x6d,0x20)
DEFG(G_IMediaObject,        0xd8ad0f58,0x5494,0x4102, 0x97,0xc5,0xec,0x79,0x8e,0x59,0xbc,0xf4)
DEFG(G_IMediaObjectInPlace, 0x651b9ad0,0x0fc7,0x4aa9, 0x95,0x38,0xd8,0x99,0x31,0x01,0x07,0x41)
DEFG(G_IDMOQualityControl,  0x65abea96,0xcf36,0x453f, 0xaf,0x8a,0x70,0x5e,0x98,0xf1,0x62,0x60)
DEFG(G_IDMOVideoOutputOpt,  0xbe8f4f4e,0x5b16,0x4d29, 0xb3,0x50,0x7f,0x6b,0x5d,0x92,0x98,0xac)
DEFG(G_CLSID_WMAsfReader,   0x187463a0,0x5bb7,0x11d3, 0xac,0xbe,0x00,0x80,0xc7,0x5e,0x24,0x6e)

static inline bool guid_eq(REFGUID a, REFGUID b) { return IsEqualGUID(a, b) != 0; }

struct guid_name { const GUID *g; const char *name; };
static const guid_name g_names[] = {
    { &G_IUnknown,            "IUnknown" },
    { &G_IClassFactory,       "IClassFactory" },
    { &G_IMarshal,            "IMarshal" },
    { &G_IPersist,            "IPersist" },
    { &G_IPersistStream,      "IPersistStream" },
    { &G_IBaseFilter,         "IBaseFilter" },
    { &G_IMediaFilter,        "IMediaFilter" },
    { &G_IMediaPosition,      "IMediaPosition" },
    { &G_IBasicAudio,         "IBasicAudio" },
    { &G_IVideoWindow,        "IVideoWindow" },
    { &G_IMediaEventSink,     "IMediaEventSink" },
    { &G_IMediaSeeking,       "IMediaSeeking" },
    { &G_ISpecifyPropPages,   "ISpecifyPropertyPages" },
    { &G_IAMFilterMiscFlags,  "IAMFilterMiscFlags" },
    { &G_IPropertyBag,        "IPropertyBag" },
    { &G_IPersistPropertyBag, "IPersistPropertyBag" },
    { &G_IServiceProvider,    "IServiceProvider" },
    { &G_ISampleGrabber,      "ISampleGrabber" },
    { &G_ISampleGrabberCB,    "ISampleGrabberCB" },
    { &G_IDMOWrapperFilter,   "IDMOWrapperFilter  <<< THE ONE" },
    { &G_IMediaObject,        "IMediaObject" },
    { &G_IMediaObjectInPlace, "IMediaObjectInPlace" },
    { &G_IDMOQualityControl,  "IDMOQualityControl" },
    { &G_IDMOVideoOutputOpt,  "IDMOVideoOutputOptimizations" },
    { &G_CLSID_SampleGrabber, "CLSID_SampleGrabber" },
    { &G_CLSID_NullRenderer,  "CLSID_NullRenderer" },
    { &G_CLSID_DMOWrapper,    "CLSID_DMOWrapperFilter" },
    { &G_CLSID_WMAsfReader,   "CLSID_WMAsfReader" },
};

static void guid_str(REFGUID g, char *out, size_t n)
{
    for (size_t i = 0; i < sizeof(g_names)/sizeof(g_names[0]); i++) {
        if (guid_eq(g, *g_names[i].g)) {
            snprintf(out, n, "%s", g_names[i].name);
            return;
        }
    }
    snprintf(out, n,
        "{%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
        (unsigned long)g.Data1, g.Data2, g.Data3,
        g.Data4[0], g.Data4[1], g.Data4[2], g.Data4[3],
        g.Data4[4], g.Data4[5], g.Data4[6], g.Data4[7]);
}

/* WCHAR -> char, for filter/pin names in log lines.  Avoids relying on
 * msvcrt's %ls handling, which differs between runtimes. */
static void w2a(const WCHAR *w, char *out, int n)
{
    if (!w || n <= 0) { if (n > 0) out[0] = '\0'; return; }
    if (WideCharToMultiByte(CP_ACP, 0, w, -1, out, n, NULL, NULL) <= 0)
        out[0] = '\0';
    out[n - 1] = '\0';
}

// Logging

static CRITICAL_SECTION g_log_lock;
static HANDLE           g_log_file = INVALID_HANDLE_VALUE;
static bool             g_log_ready = false;

static void log_open(void)
{
    if (g_log_ready) return;
    g_log_ready = true;

    char quiet[8] = {0};
    if (GetEnvironmentVariableA("LA_SHIM_QUIET", quiet, sizeof(quiet)) && quiet[0] == '1')
        return;

    char path[MAX_PATH] = {0};
    if (!GetEnvironmentVariableA("LA_SHIM_LOG", path, sizeof(path)) || !path[0])
        lstrcpynA(path, "C:\\lashim.log", sizeof(path));

    g_log_file = CreateFileA(path, FILE_APPEND_DATA, FILE_SHARE_READ|FILE_SHARE_WRITE,
                             NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
}

static void logf(const char *fmt, ...)
{
    char body[1024];
    char line[1152];
    va_list ap;

    va_start(ap, fmt);
    vsnprintf(body, sizeof(body), fmt, ap);
    va_end(ap);

    snprintf(line, sizeof(line), "[lashim %5lu.%03lu tid %04lx] %s\n",
             GetTickCount() / 1000, GetTickCount() % 1000,
             GetCurrentThreadId(), body);

    OutputDebugStringA(line);

    EnterCriticalSection(&g_log_lock);
    log_open();
    if (g_log_file != INVALID_HANDLE_VALUE) {
        DWORD written = 0;
        WriteFile(g_log_file, line, (DWORD)lstrlenA(line), &written, NULL);
        FlushFileBuffers(g_log_file);
    }
    LeaveCriticalSection(&g_log_lock);
}


// Interfaces

#ifndef __ISampleGrabberCB_INTERFACE_DEFINED__
#define __ISampleGrabberCB_INTERFACE_DEFINED__
struct ISampleGrabberCB : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE SampleCB(double SampleTime, IMediaSample *pSample) = 0;
    virtual HRESULT STDMETHODCALLTYPE BufferCB(double SampleTime, BYTE *pBuffer, LONG BufferLen) = 0;
};
#endif

#ifndef __ISampleGrabber_INTERFACE_DEFINED__
#define __ISampleGrabber_INTERFACE_DEFINED__
struct ISampleGrabber : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE SetOneShot(BOOL OneShot) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetMediaType(const AM_MEDIA_TYPE *pType) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType(AM_MEDIA_TYPE *pType) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetBufferSamples(BOOL BufferThem) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentBuffer(LONG *pBufferSize, LONG *pBuffer) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentSample(IMediaSample **ppSample) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetCallback(ISampleGrabberCB *pCallback, LONG WhichMethodToCallback) = 0;
};
#endif

#ifndef __IDMOWrapperFilter_INTERFACE_DEFINED__
#define __IDMOWrapperFilter_INTERFACE_DEFINED__
struct IDMOWrapperFilter : public IUnknown
{
    virtual HRESULT STDMETHODCALLTYPE Init(REFCLSID clsidDMO, REFCLSID catDMO) = 0;
};
#endif

// Module state

static LONG     g_objects = 0;
static LONG     g_locks   = 0;
static HMODULE  g_module  = NULL;

/* Create the genuine qedit sample grabber without going through the
 * registry */
static HRESULT create_real_grabber(IUnknown **out)
{
    typedef HRESULT (WINAPI *pfn_dgco)(REFCLSID, REFIID, void **);

    *out = NULL;

    HMODULE qedit = LoadLibraryW(L"qedit.dll");
    if (!qedit) {
        logf("FATAL: LoadLibrary(qedit.dll) failed, err %lu", GetLastError());
        return E_FAIL;
    }

    pfn_dgco dgco = (pfn_dgco)(void *)GetProcAddress(qedit, "DllGetClassObject");
    if (!dgco) {
        logf("FATAL: qedit.dll has no DllGetClassObject");
        return E_FAIL;
    }

    IClassFactory *cf = NULL;
    HRESULT hr = dgco(G_CLSID_SampleGrabber, G_IClassFactory, (void **)&cf);
    if (FAILED(hr)) {
        logf("FATAL: qedit DllGetClassObject(CLSID_SampleGrabber) -> 0x%08lx", (unsigned long)hr);
        return hr;
    }

    hr = cf->CreateInstance(NULL, G_IUnknown, (void **)out);
    cf->Release();

    if (FAILED(hr))
        logf("FATAL: real SampleGrabber CreateInstance -> 0x%08lx", (unsigned long)hr);

    return hr;
}

static void hide_video_windows(IFilterGraph *graph)
{
    if (!graph) return;

    IEnumFilters *filters = NULL;
    if (FAILED(graph->EnumFilters(&filters)) || !filters) {
        logf("hide_video_windows: EnumFilters failed");
        return;
    }

    int found = 0;

    IBaseFilter *f = NULL;
    ULONG got = 0;
    while (filters->Next(1, &f, &got) == S_OK && f) {
        IVideoWindow *vw = NULL;
        if (SUCCEEDED(f->QueryInterface(G_IVideoWindow, (void **)&vw)) && vw) {
            char nm[160] = {0};
            FILTER_INFO fi;
            memset(&fi, 0, sizeof(fi));
            if (SUCCEEDED(f->QueryFilterInfo(&fi))) {
                w2a(fi.achName, nm, sizeof(nm));
                if (fi.pGraph) fi.pGraph->Release();
            }
            HRESULT h1 = vw->put_AutoShow(OAFALSE);
            HRESULT h2 = vw->put_Visible(OAFALSE);
            HRESULT h3 = vw->put_WindowState(SW_HIDE);
            logf("hiding video window on \"%s\": AutoShow -> 0x%08lx, "
                 "Visible -> 0x%08lx, WindowState -> 0x%08lx",
                 nm, (unsigned long)h1, (unsigned long)h2, (unsigned long)h3);
            vw->Release();
            found++;
        }
        f->Release();
        f = NULL;
        got = 0;
    }
    filters->Release();

    if (!found)
        logf("hide_video_windows: no filter in the graph exposes IVideoWindow");
}

static bool contains_ci(const char *hay, const char *needle)
{
    size_t hn = strlen(hay), nn = strlen(needle);
    if (!nn || nn > hn) return false;
    for (size_t i = 0; i + nn <= hn; i++) {
        size_t j = 0;
        while (j < nn) {
            char a = hay[i + j], b = needle[j];
            if (a >= 'A' && a <= 'Z') a = (char)(a + 32);
            if (b >= 'A' && b <= 'Z') b = (char)(b + 32);
            if (a != b) break;
            j++;
        }
        if (j == nn) return true;
    }
    return false;
}

static BOOL CALLBACK hide_window_proc(HWND hwnd, LPARAM lp)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;

    char cls[160] = {0};
    GetClassNameA(hwnd, cls, sizeof(cls));
    BOOL vis = IsWindowVisible(hwnd);

    /* Wine's quartz video renderer uses the class "wine_quartz_window"
     * (confirmed from a window scan on the Deck).  "ActiveMovie" is what
     * real DirectShow uses, kept so this also works off Wine. */
    if (contains_ci(cls, "wine_quartz_window") ||
        contains_ci(cls, "activemovie") ||
        contains_ci(cls, "video renderer")) {
        ShowWindowAsync(hwnd, SW_HIDE);
        logf("  window %p class=\"%s\" visible=%d -> SW_HIDE posted",
             (void *)hwnd, cls, (int)vis);
        (*(int *)lp)++;
    } else if (vis) {
        /* Logged so that, if the match ever misses, we can see what the
         * real class name is instead of guessing again. */
        logf("  window %p class=\"%s\" visible=1 (left alone)", (void *)hwnd, cls);
    }
    return TRUE;
}

static void hide_movie_windows_by_handle(const char *where)
{
    int hidden = 0;
    logf("scanning top-level windows (%s):", where);
    EnumWindows(hide_window_proc, (LPARAM)&hidden);
    if (!hidden)
        logf("  no ActiveMovie-class window found");
}

// Wrapper

class GrabberShim : public IBaseFilter,
                    public ISampleGrabber,
                    public IDMOWrapperFilter
{
public:
    GrabberShim()
        : m_ref(1), m_inner(NULL), m_innerFilter(NULL), m_innerGrabber(NULL),
          m_graph(NULL)
    {
        InterlockedIncrement(&g_objects);
    }

    HRESULT construct()
    {
        HRESULT hr = create_real_grabber(&m_inner);
        if (FAILED(hr)) return hr;

        hr = m_inner->QueryInterface(G_IBaseFilter, (void **)&m_innerFilter);
        if (FAILED(hr)) {
            logf("FATAL: inner grabber has no IBaseFilter (0x%08lx)", (unsigned long)hr);
            return hr;
        }
        hr = m_inner->QueryInterface(G_ISampleGrabber, (void **)&m_innerGrabber);
        if (FAILED(hr)) {
            logf("FATAL: inner grabber has no ISampleGrabber (0x%08lx)", (unsigned long)hr);
            return hr;
        }
        logf("shim %p wrapping real SampleGrabber %p", (void *)this, (void *)m_inner);
        return S_OK;
    }

    // IUnknown (overrides all three base subobjects)

    STDMETHODIMP QueryInterface(REFIID riid, void **ppv) override
    {
        char n[128];
        guid_str(riid, n, sizeof(n));

        if (!ppv) return E_POINTER;
        *ppv = NULL;

        const char *how = NULL;

        if (guid_eq(riid, G_IUnknown) || guid_eq(riid, G_IPersist) ||
            guid_eq(riid, G_IMediaFilter) || guid_eq(riid, G_IBaseFilter)) {
            *ppv = static_cast<IBaseFilter *>(this);
            how  = "shim IBaseFilter";
        } else if (guid_eq(riid, G_ISampleGrabber)) {
            *ppv = static_cast<ISampleGrabber *>(this);
            how  = "shim ISampleGrabber";
        } else if (guid_eq(riid, G_IDMOWrapperFilter)) {
            *ppv = static_cast<IDMOWrapperFilter *>(this);
            how  = "shim IDMOWrapperFilter *** the game found its wrapper ***";
        }

        if (*ppv) {
            AddRef();
            logf("QI %-40s -> S_OK (%s)", n, how);
            return S_OK;
        }

        HRESULT hr = m_inner->QueryInterface(riid, ppv);
        if (SUCCEEDED(hr)) {
            logf("QI %-40s -> S_OK (delegated to inner grabber)", n);
            return hr;
        }

        logf("QI %-40s -> E_NOINTERFACE", n);
        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() override
    {
        return (ULONG)InterlockedIncrement(&m_ref);
    }

    STDMETHODIMP_(ULONG) Release() override
    {
        LONG r = InterlockedDecrement(&m_ref);
        if (r == 0) {
            logf("shim %p destroyed", (void *)this);
            delete this;
        }
        return (ULONG)r;
    }

    // IPersist

    STDMETHODIMP GetClassID(CLSID *pClassID) override
    { return m_innerFilter->GetClassID(pClassID); }

    // IMediaFilter

    STDMETHODIMP Stop() override
    {
        logf("IMediaFilter::Stop");
        release_downstream();
        HRESULT hr = m_innerFilter->Stop();
        logf("IMediaFilter::Stop inner -> 0x%08lx", (unsigned long)hr);
        hide_movie_windows_by_handle("Stop");
        return hr;
    }

    STDMETHODIMP Pause() override
    {
        logf("IMediaFilter::Pause");
        hide_movie_windows_by_handle("Pause");
        return m_innerFilter->Pause();
    }

    STDMETHODIMP Run(REFERENCE_TIME tStart) override
    {
        logf("IMediaFilter::Run(%lld)", (long long)tStart);
        hide_movie_windows_by_handle("Run");
        return m_innerFilter->Run(tStart);
    }

    STDMETHODIMP GetState(DWORD ms, FILTER_STATE *pState) override
    { return m_innerFilter->GetState(ms, pState); }

    STDMETHODIMP SetSyncSource(IReferenceClock *pClock) override
    { return m_innerFilter->SetSyncSource(pClock); }

    STDMETHODIMP GetSyncSource(IReferenceClock **ppClock) override
    { return m_innerFilter->GetSyncSource(ppClock); }

    //IBaseFilter

    STDMETHODIMP EnumPins(IEnumPins **ppEnum) override
    { return m_innerFilter->EnumPins(ppEnum); }

    STDMETHODIMP FindPin(LPCWSTR Id, IPin **ppPin) override
    { return m_innerFilter->FindPin(Id, ppPin); }

    STDMETHODIMP QueryFilterInfo(FILTER_INFO *pInfo) override
    { return m_innerFilter->QueryFilterInfo(pInfo); }

    STDMETHODIMP JoinFilterGraph(IFilterGraph *pGraph, LPCWSTR pName) override
    {
        logf("JoinFilterGraph(graph %p)", (void *)pGraph);
        m_graph = pGraph;
        HRESULT hr = m_innerFilter->JoinFilterGraph(pGraph, pName);
        if (pGraph) {
            hide_video_windows(pGraph);
            hide_movie_windows_by_handle("JoinFilterGraph");
        }
        return hr;
    }

    STDMETHODIMP QueryVendorInfo(LPWSTR *pVendorInfo) override
    { return m_innerFilter->QueryVendorInfo(pVendorInfo); }

    // ISampleGrabber

    STDMETHODIMP SetOneShot(BOOL b) override
    { logf("ISampleGrabber::SetOneShot(%d)", (int)b); return m_innerGrabber->SetOneShot(b); }

    STDMETHODIMP SetMediaType(const AM_MEDIA_TYPE *pType) override
    {
        if (pType) {
            char maj[128], sub[128];
            guid_str(pType->majortype, maj, sizeof(maj));
            guid_str(pType->subtype,   sub, sizeof(sub));
            logf("ISampleGrabber::SetMediaType major=%s sub=%s", maj, sub);
        }
        return m_innerGrabber->SetMediaType(pType);
    }

    STDMETHODIMP GetConnectedMediaType(AM_MEDIA_TYPE *pType) override
    { return m_innerGrabber->GetConnectedMediaType(pType); }

    STDMETHODIMP SetBufferSamples(BOOL b) override
    { logf("ISampleGrabber::SetBufferSamples(%d)", (int)b); return m_innerGrabber->SetBufferSamples(b); }

    STDMETHODIMP GetCurrentBuffer(LONG *pSize, LONG *pBuf) override
    { return m_innerGrabber->GetCurrentBuffer(pSize, pBuf); }

    STDMETHODIMP GetCurrentSample(IMediaSample **ppSample) override
    { return m_innerGrabber->GetCurrentSample(ppSample); }

    STDMETHODIMP SetCallback(ISampleGrabberCB *pCB, LONG which) override
    {
        logf("ISampleGrabber::SetCallback(%p, %ld)", (void *)pCB, (long)which);
        return m_innerGrabber->SetCallback(pCB, which);
    }

    // IDMOWrapperFilter

    STDMETHODIMP Init(REFCLSID clsidDMO, REFCLSID catDMO) override
    {
        char c[128], k[128];
        guid_str(clsidDMO, c, sizeof(c));
        guid_str(catDMO,   k, sizeof(k));
        logf("=== IDMOWrapperFilter::Init(dmo=%s, category=%s) -> S_OK ===", c, k);
        return S_OK;
    }

private:
    void release_downstream()
    {
        IEnumPins *pins = NULL;
        if (FAILED(m_innerFilter->EnumPins(&pins)) || !pins) {
            logf("Stop: EnumPins failed, skipping downstream release");
            return;
        }

        IPin *pin = NULL;
        ULONG got = 0;
        while (pins->Next(1, &pin, &got) == S_OK && pin) {
            PIN_DIRECTION dir = PINDIR_INPUT;
            if (SUCCEEDED(pin->QueryDirection(&dir)) && dir == PINDIR_OUTPUT)
                release_via_pin(pin);
            pin->Release();
            pin = NULL;
            got = 0;
        }
        pins->Release();
    }

    void release_via_pin(IPin *out)
    {
        IPin *peer = NULL;
        HRESULT hr = out->ConnectedTo(&peer);
        if (FAILED(hr) || !peer) {
            logf("Stop: output pin not connected (0x%08lx)", (unsigned long)hr);
            return;
        }

        PIN_INFO pi;
        memset(&pi, 0, sizeof(pi));
        if (FAILED(peer->QueryPinInfo(&pi)) || !pi.pFilter) {
            logf("Stop: QueryPinInfo on downstream pin failed");
            peer->Release();
            return;
        }

        char name[160] = {0};
        FILTER_INFO fi;
        memset(&fi, 0, sizeof(fi));
        if (SUCCEEDED(pi.pFilter->QueryFilterInfo(&fi))) {
            w2a(fi.achName, name, sizeof(name));
            if (fi.pGraph) fi.pGraph->Release();
        }

        logf("Stop: releasing downstream filter \"%s\" (%p)", name, (void *)pi.pFilter);

        hr = pi.pFilter->Stop();
        logf("Stop: downstream Stop -> 0x%08lx", (unsigned long)hr);

        pi.pFilter->Release();
        peer->Release();
    }

    ~GrabberShim()
    {
        if (m_innerGrabber) m_innerGrabber->Release();
        if (m_innerFilter)  m_innerFilter->Release();
        if (m_inner)        m_inner->Release();
        InterlockedDecrement(&g_objects);
    }

    LONG               m_ref;
    IUnknown          *m_inner;
    IBaseFilter       *m_innerFilter;
    ISampleGrabber    *m_innerGrabber;
    IFilterGraph      *m_graph;
};



class ShimFactory : public IClassFactory
{
public:
    STDMETHODIMP QueryInterface(REFIID riid, void **ppv) override
    {
        if (!ppv) return E_POINTER;
        if (guid_eq(riid, G_IUnknown) || guid_eq(riid, G_IClassFactory)) {
            *ppv = static_cast<IClassFactory *>(this);
            AddRef();
            return S_OK;
        }
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() override  { return 2; }  /* static singleton */
    STDMETHODIMP_(ULONG) Release() override { return 1; }

    STDMETHODIMP CreateInstance(IUnknown *outer, REFIID riid, void **ppv) override
    {
        char n[128];
        guid_str(riid, n, sizeof(n));
        logf("factory CreateInstance(riid=%s)", n);

        if (!ppv) return E_POINTER;
        *ppv = NULL;
        if (outer) return CLASS_E_NOAGGREGATION;

        GrabberShim *obj = new GrabberShim();
        if (!obj) return E_OUTOFMEMORY;

        HRESULT hr = obj->construct();
        if (FAILED(hr)) { obj->Release(); return hr; }

        hr = obj->QueryInterface(riid, ppv);
        obj->Release();
        return hr;
    }

    STDMETHODIMP LockServer(BOOL lock) override
    {
        if (lock) InterlockedIncrement(&g_locks);
        else      InterlockedDecrement(&g_locks);
        return S_OK;
    }
};

static ShimFactory g_factory;

//

extern "C" HRESULT WINAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv)
{
    char c[128];
    guid_str(rclsid, c, sizeof(c));

    if (guid_eq(rclsid, G_CLSID_SampleGrabber)) {
        logf("DllGetClassObject(%s) - serving shim factory", c);
        return g_factory.QueryInterface(riid, ppv);
    }

    logf("DllGetClassObject(%s) - not ours, CLASS_E_CLASSNOTAVAILABLE", c);
    if (ppv) *ppv = NULL;
    return CLASS_E_CLASSNOTAVAILABLE;
}

extern "C" HRESULT WINAPI DllCanUnloadNow(void)
{
    return (g_objects == 0 && g_locks == 0) ? S_OK : S_FALSE;
}

static HRESULT set_reg_string(HKEY root, const char *subkey, const char *value, const char *data)
{
    HKEY k = NULL;
    LONG r = RegCreateKeyExA(root, subkey, 0, NULL, 0, KEY_WRITE, NULL, &k, NULL);
    if (r != ERROR_SUCCESS) return HRESULT_FROM_WIN32(r);
    r = RegSetValueExA(k, value, 0, REG_SZ, (const BYTE *)data, (DWORD)(lstrlenA(data) + 1));
    RegCloseKey(k);
    return (r == ERROR_SUCCESS) ? S_OK : HRESULT_FROM_WIN32(r);
}

static const char *const SG_KEY =
    "Software\\Classes\\CLSID\\{c1f400a0-3f08-11d3-9f0b-006008039e37}\\InprocServer32";

static bool get_reg_string(HKEY root, const char *subkey, const char *value,
                           char *out, DWORD n)
{
    HKEY k = NULL;
    out[0] = '\0';
    if (RegOpenKeyExA(root, subkey, 0, KEY_READ, &k) != ERROR_SUCCESS) return false;
    DWORD type = 0, cb = n;
    LONG r = RegQueryValueExA(k, value, NULL, &type, (BYTE *)out, &cb);
    RegCloseKey(k);
    if (r != ERROR_SUCCESS || type != REG_SZ) { out[0] = '\0'; return false; }
    out[(cb < n) ? cb : n - 1] = '\0';
    return true;
}

extern "C" HRESULT WINAPI DllRegisterServer(void)
{
    char path[MAX_PATH] = {0};
    if (!GetModuleFileNameA(g_module, path, sizeof(path)))
        return HRESULT_FROM_WIN32(GetLastError());

    char prev[MAX_PATH] = {0};
    if (get_reg_string(HKEY_LOCAL_MACHINE, SG_KEY, NULL, prev, sizeof(prev))
        && prev[0] && lstrcmpiA(prev, path) != 0)
    {
        set_reg_string(HKEY_LOCAL_MACHINE, SG_KEY, "LashimPrevious", prev);
        logf("DllRegisterServer: saved previous InprocServer32 = %s", prev);
    }

    HRESULT hr = set_reg_string(HKEY_LOCAL_MACHINE, SG_KEY, NULL, path);
    if (FAILED(hr)) return hr;
    hr = set_reg_string(HKEY_LOCAL_MACHINE, SG_KEY, "ThreadingModel", "Both");
    if (FAILED(hr)) return hr;

    logf("DllRegisterServer: CLSID_SampleGrabber -> %s", path);
    return S_OK;
}

extern "C" HRESULT WINAPI DllUnregisterServer(void)
{
    char prev[MAX_PATH] = {0};
    if (!get_reg_string(HKEY_LOCAL_MACHINE, SG_KEY, "LashimPrevious", prev, sizeof(prev))
        || !prev[0])
        lstrcpynA(prev, "C:\\windows\\system32\\qedit.dll", sizeof(prev));

    HRESULT hr = set_reg_string(HKEY_LOCAL_MACHINE, SG_KEY, NULL, prev);
    if (FAILED(hr)) return hr;

    HKEY k = NULL;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, SG_KEY, 0, KEY_SET_VALUE, &k) == ERROR_SUCCESS) {
        RegDeleteValueA(k, "LashimPrevious");
        RegCloseKey(k);
    }

    logf("DllUnregisterServer: CLSID_SampleGrabber restored to %s", prev);
    return S_OK;
}

BOOL WINAPI DllMain(HINSTANCE inst, DWORD reason, LPVOID reserved)
{
    switch (reason) {
    case DLL_PROCESS_ATTACH:
        g_module = (HMODULE)inst;
        InitializeCriticalSection(&g_log_lock);
        DisableThreadLibraryCalls(inst);
        logf("---- lashim loaded into pid %lu ----", GetCurrentProcessId());
        break;
    case DLL_PROCESS_DETACH:
        if (!reserved) {
            EnterCriticalSection(&g_log_lock);
            if (g_log_file != INVALID_HANDLE_VALUE) {
                CloseHandle(g_log_file);
                g_log_file = INVALID_HANDLE_VALUE;
            }
            g_log_ready = false;
            LeaveCriticalSection(&g_log_lock);
            DeleteCriticalSection(&g_log_lock);
        }
        break;
    }
    return TRUE;
}
