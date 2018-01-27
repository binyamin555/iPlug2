#include "IPlugEffect.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"
#include "config.h"

#include "IPlugEffectControls.h"

IPlugEffect::IPlugEffect(IPlugInstanceInfo instanceInfo)
: IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo)
{
  TRACE; 

  GetParam(kGain)->InitDouble("Gain", 0., 0., 100.0, 0.01, "%");

  IGraphics* pGraphics = MakeGraphics(*this, kWidth, kHeight, 60);
  pGraphics->AttachPanelBackground(COLOR_RED);
  
  
  // lamda function for custom actions on stock IControls
  pGraphics->AttachControl(new IVSwitchControl(*this, IRECT(100, 100, 150, 150), kNoParameter, [pGraphics](IControl* pCaller)
  {
    pCaller->Hide(pGraphics->ShowMessageBox("hide that box?", "", MB_YESNO) == IDYES);
  }));
  
  ISVG svg = pGraphics->LoadSVG("C:\\Program Files\\VCV\\Rack\\res\\ComponentLibrary\\BefacoBigKnob.svg"); // load initial svg, can be a resource or absolute path
  
  SVGKnob* knob = new SVGKnob(*this, 200, 200, svg, kGain);
  
  pGraphics->AttachControl(new FileMenu(*this, IRECT(10,0,280,30), [pGraphics, knob](IControl* pCaller)
                                        {
                                          WDL_String path;
                                          dynamic_cast<IDirBrowseControlBase*>(pCaller)->GetSelecteItemPath(path);
                                          ISVG svg = pGraphics->LoadSVG(path.Get());
                                          knob->SetSVG(svg);
                                        }, "C:\\Program Files\\VCV\\Rack\\res\\ComponentLibrary"));

  //pGraphics->AttachControl(new IVKeyboardControl(*this, IRECT(0, kHeight - 100, kWidth, kHeight), 36, 72));
  pGraphics->AttachControl(knob);

  AttachGraphics(pGraphics);
  
  //pGraphics->EnableLiveEdit(true);
  //pGraphics->ShowControlBounds(true);

  PrintDebugInfo();

  MakeDefaultPreset("-", kNumPrograms);
}

void IPlugEffect::ProcessBlock(double** inputs, double** outputs, int nFrames)
{
  mParams_mutex.Enter();
  const double gain = GetParam(kGain)->Value() / 100.;
  mParams_mutex.Leave();
  
  double* in1 = inputs[0];
  double* in2 = inputs[1];
  double* out1 = outputs[0];
  double* out2 = outputs[1];

  for (int s = 0; s < nFrames; ++s, ++in1, ++in2, ++out1, ++out2)
  {
    *out1 = *in1 * gain;
    *out2 = *in2 * gain;
  }
}
