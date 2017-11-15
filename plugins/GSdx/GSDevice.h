/*
 *	Copyright (C) 2007-2009 Gabest
 *	http://www.gabest.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with GNU Make; see the file COPYING.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#pragma once

#include "GSFastList.h"
#include "GSWnd.h"
#include "GSTexture.h"
#include "GSVertex.h"
#include "GSAlignedClass.h"
#include "GSOsdManager.h"

enum ShaderConvert {
	ShaderConvert_COPY = 0,
	ShaderConvert_RGBA8_TO_16_BITS,
	ShaderConvert_DATM_1,
	ShaderConvert_DATM_0,
	ShaderConvert_MOD_256,
	ShaderConvert_SCANLINE = 5,
	ShaderConvert_DIAGONAL_FILTER,
	ShaderConvert_TRANSPARENCY_FILTER,
	ShaderConvert_TRIANGULAR_FILTER,
	ShaderConvert_COMPLEX_FILTER,
	ShaderConvert_FLOAT32_TO_32_BITS = 10,
	ShaderConvert_FLOAT32_TO_RGBA8,
	ShaderConvert_FLOAT16_TO_RGB5A1,
	ShaderConvert_RGBA8_TO_FLOAT32 = 13,
	ShaderConvert_RGBA8_TO_FLOAT24,
	ShaderConvert_RGBA8_TO_FLOAT16,
	ShaderConvert_RGB5A1_TO_FLOAT16,
	ShaderConvert_RGBA_TO_8I = 17,
	ShaderConvert_OSD,
	ShaderConvert_YUV,
	ShaderConvert_Count
};

#pragma pack(push, 1)

class MergeConstantBuffer
{
public:
	GSVector4 BGColor;

	MergeConstantBuffer() {memset(this, 0, sizeof(*this));}
};

class InterlaceConstantBuffer
{
public:
	GSVector2 ZrH;
	float hH;
	float _pad[1];

	InterlaceConstantBuffer() {memset(this, 0, sizeof(*this));}
};

class ExternalFXConstantBuffer
{
public:
	GSVector2 xyFrame;
	GSVector4 rcpFrame;
	GSVector4 rcpFrameOpt;

	ExternalFXConstantBuffer() { memset(this, 0, sizeof(*this)); }
};

class FXAAConstantBuffer
{
public:
	GSVector4 rcpFrame;
	GSVector4 rcpFrameOpt;

	FXAAConstantBuffer() {memset(this, 0, sizeof(*this));}
};

class ShadeBoostConstantBuffer
{
public:
	GSVector4 rcpFrame;
	GSVector4 rcpFrameOpt;

	ShadeBoostConstantBuffer() {memset(this, 0, sizeof(*this));}
};

#pragma pack(pop)

class GSDevice : public GSAlignedClass<32>
{
	FastList<GSTexture*> m_pool;

protected:
	std::shared_ptr<GSWnd> m_wnd;
	int m_vsync;
	bool m_rbswapped;
	GSTexture* m_backbuffer;
	GSTexture* m_merge;
	GSTexture* m_weavebob;
	GSTexture* m_blend;
	GSTexture* m_shaderfx;
	GSTexture* m_fxaa;
	GSTexture* m_shadeboost;
	GSTexture* m_1x1;
	GSTexture* m_current;
	struct {size_t stride, start, count, limit;} m_vertex;
	struct {size_t start, count, limit;} m_index;
	unsigned int m_frame; // for ageing the pool
	bool m_linear_present;

	virtual GSTexture* CreateSurface(int type, int w, int h, bool msaa, int format) = 0;
	virtual GSTexture* FetchSurface(int type, int w, int h, bool msaa, int format);

	virtual void DoMerge(GSTexture* sTex[3], GSVector4* sRect, GSTexture* dTex, GSVector4* dRect, const GSRegPMODE& PMODE, const GSRegEXTBUF& EXTBUF, const GSVector4& c) = 0;
	virtual void DoInterlace(GSTexture* sTex, GSTexture* dTex, int shader, bool linear, float yoffset) = 0;
	virtual void DoFXAA(GSTexture* sTex, GSTexture* dTex) {}
	virtual void DoShadeBoost(GSTexture* sTex, GSTexture* dTex) {}
	virtual void DoExternalFX(GSTexture* sTex, GSTexture* dTex) {}

public:
	GSOsdManager m_osd;

	GSDevice();
	virtual ~GSDevice();

	void Recycle(GSTexture* t);

	enum {Windowed, Fullscreen, DontCare};

	virtual bool Create(const std::shared_ptr<GSWnd> &wnd);
	virtual bool Reset(int w, int h);
	virtual bool IsLost(bool update = false) {return false;}
	virtual void Present(const GSVector4i& r, int shader);
	virtual void Present(GSTexture* sTex, GSTexture* dTex, const GSVector4& dRect, int shader = 0);
	virtual void Flip() {}

	virtual void SetVSync(int vsync) {m_vsync = vsync;}

	virtual void BeginScene() {}
	virtual void DrawPrimitive() {};
	virtual void DrawIndexedPrimitive() {}
	virtual void DrawIndexedPrimitive(int offset, int count) {}
	virtual void EndScene();

	virtual void ClearRenderTarget(GSTexture* t, const GSVector4& c) {}
	virtual void ClearRenderTarget(GSTexture* t, uint32 c) {}
	virtual void ClearDepth(GSTexture* t) {}
	virtual void ClearStencil(GSTexture* t, uint8 c) {}

	virtual GSTexture* CreateRenderTarget(int w, int h, bool msaa, int format = 0);
	virtual GSTexture* CreateDepthStencil(int w, int h, bool msaa, int format = 0);
	virtual GSTexture* CreateTexture(int w, int h, int format = 0);
	virtual GSTexture* CreateOffscreen(int w, int h, int format = 0);

	virtual GSTexture* Resolve(GSTexture* t) {return NULL;}

	virtual GSTexture* CopyOffscreen(GSTexture* src, const GSVector4& sRect, int w, int h, int format = 0, int ps_shader = 0) {return NULL;}

	virtual void CopyRect(GSTexture* sTex, GSTexture* dTex, const GSVector4i& r) {}
	virtual void StretchRect(GSTexture* sTex, const GSVector4& sRect, GSTexture* dTex, const GSVector4& dRect, int shader = 0, bool linear = true) {}

	void StretchRect(GSTexture* sTex, GSTexture* dTex, const GSVector4& dRect, int shader = 0, bool linear = true);

	virtual void PSSetShaderResources(GSTexture* sr0, GSTexture* sr1) {}
	virtual void PSSetShaderResource(int i, GSTexture* sRect) {}
	virtual void OMSetRenderTargets(GSTexture* rt, GSTexture* ds, const GSVector4i* scissor = NULL) {}

	GSTexture* GetCurrent();

	void Merge(GSTexture* sTex[3], GSVector4* sRect, GSVector4* dRect, const GSVector2i& fs, const GSRegPMODE& PMODE, const GSRegEXTBUF& EXTBUF, const GSVector4& c);
	void Interlace(const GSVector2i& ds, int field, int mode, float yoffset);
	void FXAA();
	void ShadeBoost();
	void ExternalFX();
	virtual void RenderOsd(GSTexture* dt) {};

	bool ResizeTexture(GSTexture** t, int w, int h);

	bool IsRBSwapped() {return m_rbswapped;}

	void AgePool();
	void PurgePool();

	virtual void PrintMemoryUsage();
};

struct GSAdapter
{
	uint32 vendor;
	uint32 device;
	uint32 subsys;
	uint32 rev;

	operator std::string() const;
	bool operator==(const GSAdapter&) const;
	bool operator==(const std::string &s) const
	{
		return (std::string)*this == s;
	}
	bool operator==(const char *s) const
	{
		return (std::string)*this == s;
	}

#ifdef _WIN32
	GSAdapter(const DXGI_ADAPTER_DESC1 &desc_dxgi);
	GSAdapter(const D3DADAPTER_IDENTIFIER9 &desc_d3d9);
#endif
#ifdef __linux__
	// TODO
#endif
};
