Примеры работы

```
$ ./UDPClient
Usage: ip port (-i | command)
```

```
$ ./UDPClient 192.168.0.10 3333 LED1On
Server responded 01LD11
```

```
$ ./UDPClient 192.168.0.10 3333 "LED1Off LED2On"
Server responded 01LD10LD21
```

Интерактивный режим:
```
$ ./UDPClient 192.168.0.10 3333 -i
> LED2Off
Server responded 01LD20
> LED1On LED3On
Server responded 01LD11LD31
> LED1Off LED2On LED3Off
Server responded 01LD10LD21LD30
> exit
```


