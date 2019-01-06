#!/bin/bash
set -v

if "%1" == "before_install" ( 

) else if "%1" == "install" (
  choco install qtcreator
)
