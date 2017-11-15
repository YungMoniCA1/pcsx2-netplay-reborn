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

#include "GSRendererHW.h"
#include "GSDeviceDX.h"

class GSRendererDX : public GSRendererHW
{
	GSVector2 m_pixelcenter;
	bool m_logz;
	bool m_fba;

	bool UserHacks_AlphaHack;
	bool UserHacks_AlphaStencil;

protected:
	void EmulateAtst(const int pass, const GSTextureCache::Source* tex);
	void EmulateZbuffer();
	void EmulateTextureSampler(const GSTextureCache::Source* tex);
	virtual void DrawPrims(GSTexture* rt, GSTexture* ds, GSTextureCache::Source* tex);
	virtual void SetupIA(const float& sx, const float& sy) = 0;
	virtual void UpdateFBA(GSTexture* rt) {}

	unsigned int UserHacks_TCOffset;
	float UserHacks_TCO_x, UserHacks_TCO_y;
	int UserHacks_HPO;

	bool DATE;

	GSDrawingContext* context;
	GSDrawingEnvironment env;

	GSDeviceDX* dev;

	GSDeviceDX::OMDepthStencilSelector om_dssel;
	GSDeviceDX::OMBlendSelector om_bsel;

	GSDeviceDX::PSSelector m_ps_sel;
	GSDeviceDX::PSSamplerSelector m_ps_ssel;
	GSDeviceDX::GSSelector m_gs_sel;

	GSDeviceDX::PSConstantBuffer ps_cb;
	GSDeviceDX::VSConstantBuffer vs_cb;
	GSDeviceDX::GSConstantBuffer gs_cb;

public:
	GSRendererDX(GSTextureCache* tc, const GSVector2& pixelcenter = GSVector2(0));
	virtual ~GSRendererDX();

};
