#include <pxl/asset/ImageLoader.hpp>

#include <cstddef>
#include <memory>
#include <utility>

#include <pxl/platform/Fetch.hpp>

namespace pxl::asset
{
    void LoadImage(const std::string& url, ImageCallback callback)
    {
        platform::FetchFile(url, [callback = std::move(callback)](bool success, std::span<const std::byte> data)
        {
            callback(success ? DecodeImage(data) : Image{});
        });
    }

    void LoadImages(const std::vector<std::string>& urls, ImagesCallback callback)
    {
        if (urls.empty())
        {
            callback({});
            return;
        }

        struct Batch
        {
                std::vector<Image> images;
                size_t pending = 0;
                ImagesCallback callback;
        };
        auto batch = std::make_shared<Batch>();
        batch->images.resize(urls.size());
        batch->pending = urls.size();
        batch->callback = std::move(callback);

        for (size_t i = 0; i < urls.size(); ++i)
        {
            LoadImage(urls[i], [batch, i](Image image)
            {
                batch->images[i] = std::move(image);
                if (--batch->pending == 0)
                {
                    batch->callback(std::move(batch->images));
                }
            });
        }
    }
}
