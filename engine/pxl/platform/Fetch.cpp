#include <pxl/platform/Fetch.hpp>

#include <cstring>
#include <memory>
#include <utility>

#include <emscripten/emscripten.h>
#include <emscripten/fetch.h>

#include <pxl/log/Log.hpp>

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
            log::Error("fetch {} failed: HTTP {}", fetch->url, fetch->status);
            (*onDone)(false, {});
            emscripten_fetch_close(fetch);
        }

        void OnStartFailed(void* userData)
        {
            std::unique_ptr<FetchCallback> onDone(static_cast<FetchCallback*>(userData));
            (*onDone)(false, {});
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

        if (!emscripten_fetch(&attr, url.c_str()))
        {
            log::Error("fetch {} could not be started", url);
            emscripten_async_call(OnStartFailed, attr.userData, 0);
        }
    }
}
