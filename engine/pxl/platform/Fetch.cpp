#include <pxl/platform/Fetch.hpp>

#include <cstdio>
#include <cstring>
#include <memory>
#include <utility>

#include <emscripten/fetch.h>

namespace pxl::platform
{
    namespace
    {
        void OnSuccess(emscripten_fetch_t* fetch)
        {
            std::unique_ptr<FetchCallback> onDone(static_cast<FetchCallback*>(fetch->userData));
            const auto* bytes = reinterpret_cast<const std::byte*>(fetch->data);
            (*onDone)(true, std::span<const std::byte>(bytes, static_cast<size_t>(fetch->numBytes)));
            emscripten_fetch_close(fetch);
        }

        void OnError(emscripten_fetch_t* fetch)
        {
            std::unique_ptr<FetchCallback> onDone(static_cast<FetchCallback*>(fetch->userData));
            std::fprintf(stderr, "[pxl] fetch %s failed: HTTP %u\n", fetch->url, static_cast<unsigned>(fetch->status));
            (*onDone)(false, {});
            emscripten_fetch_close(fetch);
        }
    }

    void FetchFile(const std::string& url, FetchCallback callback)
    {
        emscripten_fetch_attr_t attr;
        emscripten_fetch_attr_init(&attr);
        std::strcpy(attr.requestMethod, "GET");
        attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
        attr.onsuccess = OnSuccess;
        attr.onerror = OnError;
        attr.userData = new FetchCallback(std::move(callback));
        emscripten_fetch(&attr, url.c_str());
    }
}
