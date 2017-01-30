#ifndef QMTLRENDERDEVICE_H
#define QMTLRENDERDEVICE_H
#include <NsRender/RenderDevice.h>
#include <NsCore/PtrForward.h>
#include <NsCore/HashMap.h>

#include <memory>

namespace Noesis {
namespace Render {

class MTLTexture;

class QNsRenderDevice final: public RenderDevice
{
public:
    QNsRenderDevice();
    virtual ~QNsRenderDevice();

private:
    const DeviceCaps& GetCaps() const;

    Ptr<RenderTarget> CreateRenderTarget(const NsChar* label, NsSize width, NsSize height,
        NsSize sampleCount);

    Ptr<RenderTarget> CloneRenderTarget(const NsChar* label, RenderTarget* surface);

    Ptr<Texture> CreateTexture(const NsChar* label, NsSize width, NsSize height,
        NsSize numLevels, TextureFormat::Enum format);

    void UpdateTexture(Texture* texture, NsSize level, NsSize x, NsSize y, NsSize width,
        NsSize height, const void* data);

    void BeginRender(NsBool offscreen);
    void SetRenderTarget(RenderTarget* surface);
    void BeginTile(const Tile& tile, NsSize surfaceWidth, NsSize surfaceHeight);
    void EndTile();
    void ResolveRenderTarget(RenderTarget* surface, const Tile* tiles, NsSize numTiles);
    void EndRender();
    void* MapVertices(NsSize bytes);
    void UnmapVertices();
    void* MapIndices(NsSize bytes);
    void UnmapIndices();
    void DrawBatch(const Batch& batch);

private:
    struct QNsRenderDeviceImpl;
    std::unique_ptr<QNsRenderDeviceImpl> m_dPtr;
};

}}

#endif // QMTLRENDERDEVICE_H
