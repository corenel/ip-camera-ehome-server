%module ehome_server

%include <opencv.i>

%cv_instantiate_all_defaults

%{
  #include "config.h"
  #include "ip_camera.h"
  #include "ehome_server.h"
%}

%include "config.h"
%include "ip_camera.h"
%include "ehome_server.h"
