field oriented control: control the magnetic field to control the rotator

the magnetic field is a vector, 你可以通过SVPWM生成任意方向一定大小范围内的磁场

生成的磁场方向需与转子磁场成90度夹角以保证最大效率，即将磁力全部用于扭矩

为此，需要检测转子的位置，伺服电机通常用磁编码器进行转子的位置检测

