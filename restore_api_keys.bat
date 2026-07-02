REM xcopy /s /Y inc/ApiKeys.h.bak inc/ApiKeys.h
REM copy /Y inc/ApiKeys.h.bak inc/ApiKeys.h
REM copy inc/ApiKeys.h.bak inc/ApiKeys.h /b/v/y

REM copy /Y ".\inc\ApiKeys.h.bak" ".\inc\ApiKeys.h"
ant restore_api_keys
pause