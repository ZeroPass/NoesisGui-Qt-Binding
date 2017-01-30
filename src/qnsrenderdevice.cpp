#include "qnsrenderdevice.h"

#include <NsRender/RenderTarget.h>
#include <NsRender/Texture.h>
#include <NsCore/LoggerMacros.h>
#include <NsCore/Ptr.h>

#include <qsystemdetection.h>

#include "Src/Renderers/GL/GLRenderDevice.h"

#define rd m_dPtr->rdb

using namespace Noesis;
using namespace Noesis::Render;

struct QNsRenderDevice::QNsRenderDeviceImpl {
    QNsRenderDeviceImpl() : rdb(nullptr)
    {
        rdb = new GLRenderDevice;
    }

    ~QNsRenderDeviceImpl()
    {
        if(rdb) {
            delete rdb;
        }
    }

    RenderDevice* rdb;
};


QNsRenderDevice::QNsRenderDevice() :
    m_dPtr(std::make_unique<QNsRenderDeviceImpl>())
{}

QNsRenderDevice::~QNsRenderDevice()
{}

const DeviceCaps& QNsRenderDevice::GetCaps() const
{
    return rd->GetCaps();
}

Ptr<RenderTarget> QNsRenderDevice::CreateRenderTarget(const NsChar* label, NsSize width, NsSize height, NsSize sampleCount)
{
    return rd->CreateRenderTarget(label, width, height, sampleCount);
}

Ptr<RenderTarget> QNsRenderDevice::CloneRenderTarget(const NsChar* label, RenderTarget* surface)
{
    return rd->CloneRenderTarget(label, surface);
}

Ptr<Texture> QNsRenderDevice::CreateTexture(const NsChar* label, NsSize width, NsSize height, NsSize numLevels, TextureFormat::Enum format)
{
    return rd->CreateTexture(label, width, height, numLevels, format);
}

void QNsRenderDevice::UpdateTexture(Texture* texture, NsSize level, NsSize x, NsSize y, NsSize width, NsSize height, const void* data)
{
    rd->UpdateTexture(texture, level, x, y, width, height, data);
}

void QNsRenderDevice::BeginRender(NsBool offscreen)
{
    rd->BeginRender(offscreen);
}

void QNsRenderDevice::SetRenderTarget(RenderTarget* surface)
{
    rd->SetRenderTarget(surface);
}

void QNsRenderDevice::BeginTile(const Tile& tile, NsSize surfaceWidth, NsSize surfaceHeight)
{
    rd->BeginTile(tile, surfaceWidth, surfaceHeight);
}

void QNsRenderDevice::EndTile()
{
    rd->EndTile();
}

void QNsRenderDevice::ResolveRenderTarget(RenderTarget* surface, const Tile* tiles, NsSize numTiles)
{
    rd->ResolveRenderTarget(surface, tiles, numTiles);
}

void QNsRenderDevice::EndRender()
{
    rd->EndRender();
}

void* QNsRenderDevice::MapVertices(NsSize bytes)
{
    return rd->MapVertices(bytes);
}

void QNsRenderDevice::UnmapVertices()
{
    rd->UnmapVertices();
}

void* QNsRenderDevice::MapIndices(NsSize bytes)
{
    return rd->MapIndices(bytes);
}

void QNsRenderDevice::UnmapIndices()
{
    rd->UnmapIndices();
}

void QNsRenderDevice::DrawBatch(const Batch& batch)
{
    rd->DrawBatch(batch);
}
