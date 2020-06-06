import numpy as np
import ehome_server
import time


def main():
    s = ehome_server.EHomeServer()
    s.Start()
    camera_index = 0;
    while True:
        time.sleep(1)
        if s.IsOnline(camera_index) and \
                s.IsPushingStream(camera_index) and \
                s.IsReceivingFrame(camera_index):
            frame = np.asarray(s.GetFrame(camera_index))
            print('Received frame: {}'.format(frame.shape))
            break
    s.Stop()


if __name__ == '__main__':
    main()
