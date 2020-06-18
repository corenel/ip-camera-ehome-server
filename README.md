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

## Usage

1. Setup `LD_LIBRARY_PATH` 

   ```bash
   $ cd /path/to/this/repo
   $ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PWD/thirdparty/HCPLAYM4SDK/linux64/lib:$PWD/thirdparty/HCEHOMESDK/linux64/lib
   ```

   Or install libraries to `/usr/local`

   ```bash
   $ cd /path/to/this/repo
   $ ./scripts/install.sh
   ```

2. (Optional) Modify `include/config.h` before building

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

0. Build and install project as mentioned above

1. Clone `opencv-swig` next to this repo

   ```bash
   $ cd /path/to/this/repo
   $ git clone https://github.com/renatoGarcia/opencv-swig.git ../opencv-swig
   ```

1. Build python bindings:

   ```bash
   $ ./scripts/install_python_bindings.sh
   ```

   The generated files are in `python` directory:

   ```
   python
   ├── ehome_server.i
   ├── ehome_server.py
   ├── _ehome_server.so*
   ├── ehome_server_wrap.cxx
   ├── libices.so*
   ├── requirements.txt
   ├── setup.py
   └── test_python_bindings.py
   ```

2. Run the tests:

   ```bash
   $ cd python
   $ python3 test_python_bindings.py
   ```

## To-dos

- [ ] Test with multiple devices
- [ ] Clean code
- [ ] Add more detailed documentation
- [ ] Fix issues with ISUP SDK
- [ ] Add customizable config
