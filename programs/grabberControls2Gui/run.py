#!/usr/bin/env python3

"""
GrabberControls2Gui
---------------------

Simple GUI for controlling USB3/GigE cameras using Aravis and YARP

Author: Álvaro Santos García, David Estévez Fernández
Copyright: Universidad Carlos III de Madrid (C) 2025
CopyPolicy: Released under the terms of the GNU LGPL v2.1
"""

import sys
import begin
import yarp

from PySide6 import QtWidgets

from .GrabberControls2GuiBackend import GrabberControls2GuiBackend
from .GrabberControls2GuiGUI import GrabberControls2GuiGUI

@begin.start(auto_convert=True)
@begin.logging
def main(remote_port: 'Remote port running the AravisCamera grabber' = '/grabber'):
    yarp.Network.init()

    options = yarp.Property()
    options.put('device', 'frameGrabber_nwc_yarp')
    options.put('remote', remote_port)
    options.put('local', '/grabber/client')

    driver = yarp.PolyDriver(options)

    if not driver.isValid():
        print("ERROR: Could not connect to the device")
        return 1

    app = QtWidgets.QApplication(sys.argv)
    controls = driver.viewIFrameGrabberControls()
    backend = GrabberControls2GuiBackend(controls) if controls else None
    gui = GrabberControls2GuiGUI(backend, remote_port)
    gui.show()

    return app.exec_()
