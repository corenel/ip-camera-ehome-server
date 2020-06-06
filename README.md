# ip-camera-ehome-server
EHome server for Hikvision IP camera.

## Build

Run the following commands

```bash
$ cd /path/to/this/repo
$ mkdir build && cd build
$ cmake .. -DUSE_EHOME=ON
$ make -j
```

Or just run

```bash
$ cd /path/to/this/repo
$ ./scripts/build.sh
```

## Usage

1. Setup `LD_LIBRARY_PATH` 

   ```bash
   $ cd /path/to/this/repo
   $ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/thirdparty/HCPLAYM4SDK/linux64/lib:$PWD/thirdparty/HCEHOMESDK/linux64/lib
   ```

   Or install libraries to `/usr/local`

   ```bash
   $ cd /path/to/this/repo
   $ ./scripts/install_thirdparty_libs.sh
   ```

2. Modify `include/config.h`

3. Setup Hikvision camera (Network -> Advanced -> Advanced Platform)

   - Platform Access Type: EHome
   - Protocol Version: Ehome 4.0
   - Server Address: Public IP of EHome server (the same as `SMS_PUBLIC_IP` in `config.h`)
   - Port: Public port of EHome server (the same as `CMS_LISTEN_PORT` in `config.h`)

4. Run the demo

   ```bash
   $ cd build
   $ ./test_demo   
   ```

## Pythin bindings

1. Build python bindings:

   ```bash
   $ ./scripts/build_python_bindings.sh
   ```

   The generated files are in `python` directory:

   ```
   python
   ├── ehome_server.i
   ├── ehome_server.py
   ├── _ehome_server.so*
   ├── ehome_server_wrap.cxx
   ├── libices.so*
   └── test_python_bindings.py
   ```

2. Run the tests (ensure the environment variable  `LD_LIBRARY_PATH` is correctly set):

   ```bash
   $ python3 test_python_bindings.py
   ```


## To-dos

- [ ] Test with multiple devices
- [ ] Clean code
- [ ] Add more detailed documentation
- [ ] Fix issues with ISUP SDK