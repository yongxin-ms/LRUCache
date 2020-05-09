@set trunk_dir=%cd%
pushd %trunk_dir%

cd proto 
call protogen.bat

md %trunk_dir%\build
cd %trunk_dir%\build

cmake %trunk_dir%

popd
pause