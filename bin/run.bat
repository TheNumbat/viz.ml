@echo off

IF NOT EXIST w:\data mkdir w:\data
pushd w:\data

w:\build\viz.exe

popd
