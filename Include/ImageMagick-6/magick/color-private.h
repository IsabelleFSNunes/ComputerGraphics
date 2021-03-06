/*
  Copyright 1999-2012 ImageMagick Studio LLC, a non-profit organization
  dedicated to making software imaging solutions freely available.

  You may not use this file except in compliance with the License.
  obtain a copy of the License at

    http://www.imagemagick.org/script/license.php

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  MagickCore image color methods.
*/
#ifndef _MAGICKCORE_COLOR_PRIVATE_H
#define _MAGICKCORE_COLOR_PRIVATE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#include <magick/image.h>
#include <magick/color.h>
#include <magick/exception-private.h>
#include <magick/pixel-accessor.h>

static inline MagickBooleanType IsColorEqual(const PixelPacket *p,
  const PixelPacket *q)
{
  MagickRealType
    blue,
    green,
    red;

  red=(MagickRealType) p->red;
  green=(MagickRealType) p->green;
  blue=(MagickRealType) p->blue;
  if ((fabs(red-q->red) < MagickEpsilon) &&
      (fabs(green-q->green) < MagickEpsilon) &&
      (fabs(blue-q->blue) < MagickEpsilon))
    return(MagickTrue);
  return(MagickFalse);
}

static inline MagickBooleanType IsMagickColorEqual(const MagickPixelPacket *p,
  const MagickPixelPacket *q)
{
  if ((p->matte != MagickFalse) && (q->matte == MagickFalse) &&
      (fabs(p->opacity-OpaqueOpacity) >= MagickEpsilon))
    return(MagickFalse);
  if ((q->matte != MagickFalse) && (p->matte == MagickFalse) &&
      (fabs(q->opacity-OpaqueOpacity)) >= MagickEpsilon)
    return(MagickFalse);
  if ((p->matte != MagickFalse) && (q->matte != MagickFalse))
    {
      if (fabs(p->opacity-q->opacity) >= MagickEpsilon)
        return(MagickFalse);
      if (fabs(p->opacity-TransparentOpacity) < MagickEpsilon)
        return(MagickTrue);
    }
  if (fabs(p->red-q->red) >= MagickEpsilon)
    return(MagickFalse);
  if (fabs(p->green-q->green) >= MagickEpsilon)
    return(MagickFalse);
  if (fabs(p->blue-q->blue) >= MagickEpsilon)
    return(MagickFalse);
  if ((p->colorspace == CMYKColorspace) &&
      (fabs(p->index-q->index) >= MagickEpsilon))
    return(MagickFalse);
  return(MagickTrue);
}

static inline MagickBooleanType IsMagickGray(const MagickPixelPacket *pixel)
{
  if ((pixel->colorspace != GRAYColorspace) &&
      (pixel->colorspace != RGBColorspace))
    return(MagickFalse);
  if ((fabs(pixel->red-pixel->green) < MagickEpsilon) &&
      (fabs(pixel->green-pixel->blue) < MagickEpsilon))
    return(MagickTrue);
  return(MagickFalse);
}

static inline MagickRealType MagickPixelIntensity(
  const MagickPixelPacket *pixel)
{
  MagickRealType
    blue,
    green,
    red;

  if (pixel->colorspace == GRAYColorspace)
    return(pixel->red);
  if (pixel->colorspace != sRGBColorspace)
    return(0.298839f*pixel->red+0.586811f*pixel->green+0.114350f*pixel->blue);
  red=DecodesRGBGamma(pixel->red);
  green=DecodesRGBGamma(pixel->green);
  blue=DecodesRGBGamma(pixel->blue);
  return(0.298839f*red+0.586811f*green+0.114350f*blue);
}

static inline Quantum MagickPixelIntensityToQuantum(
  const MagickPixelPacket *pixel)
{
  MagickRealType
    blue,
    green,
    red;

  if (pixel->colorspace == GRAYColorspace)
    return(ClampToQuantum(pixel->red));
  if (pixel->colorspace != sRGBColorspace)
    return(ClampToQuantum(0.298839f*pixel->red+0.586811f*pixel->green+
      0.114350f*pixel->blue));
  red=DecodesRGBGamma(pixel->red);
  green=DecodesRGBGamma(pixel->green);
  blue=DecodesRGBGamma(pixel->blue);
  return(ClampToQuantum(0.298839f*red+0.586811f*green+0.114350f*blue));
}

static inline MagickRealType MagickPixelLuminance(
  const MagickPixelPacket *pixel)
{
  MagickRealType
    blue,
    green,
    red;

  if (pixel->colorspace == GRAYColorspace)
    return(pixel->red);
  if (pixel->colorspace != sRGBColorspace)
    return(0.21267f*pixel->red+0.71516f*pixel->green+0.07217f*pixel->blue);
  red=DecodesRGBGamma(pixel->red);
  green=DecodesRGBGamma(pixel->green);
  blue=DecodesRGBGamma(pixel->blue);
  return(0.21267f*red+0.71516f*green+0.07217f*blue);
}

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
