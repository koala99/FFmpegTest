AVFrame中的数据存放
yuv420p与yuv420的区别
区别只是在于数据在内存中的排布：
yuv420p:yyyyyyyy uuuu vvvvv
yuv420: yuv yuv yuv yuv

yuv420p在AVFrame中的存储
YUV420P(planar格式)在ffmpeg中存储是在struct AVFrame的data[]数组中
data[0]——-Y分量
data[1]——-U分量
data[2]——-V分量
linesize[]数组中保存的是对应通道的数据宽度
linesize[0]——-Y分量的宽度
linesize[1]——-U分量的宽度
---------------------
