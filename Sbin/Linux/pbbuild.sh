#!/bin/bash
protoc -I../../protocol -o ../resource/pb/server.pb ../../protocol/server.proto &&\
protoc -I../../protocol -o ../resource/pb/client.pb ../../protocol/client.proto
if [ $? -eq 0 ]
then
cp ../resource/pb/server.pb ../../protocol/server.pb &&\
cp ../resource/pb/client.pb ../../protocol/client.pb
echo "build pb success"
else
echo "build pb error"
fi


