#ifndef APPSTATUS_H
#define APPSTATUS_H

enum AppStatus
{
  FailureGLFWInit = 0,
  FailureWindow,
  FailureGLEWInit,
  FailureShaderLoad,
  FailureShaderProgramLoad,
  FailureSampleSet,
  FailureTextureLoad,
  FailureDevILStart,
  Success
};

#endif
