This explains how the dash operates using a raspberry pi logged in as dash.

There are two main parts of the dash operation, the backend and the frontend.

The backend is a set of python 3 scripts that work together to listen for data 
from the CAN shield, provide a model for the frontend, and execute shifts.

The frontend is a nw.js (node-webkit) application that presents the information
from the backend in a useful way to the driver.

LOCATION
--------

Everything related to the dash is located under ~/f26dash

The backend is located in ~/f26dash/backend

The frontend is located in ~/f26dash/frontend

The backend and frontend are launched via the system daemon, using 
dash_backend.service and dash_frontend.service, both located in
/etc/systemd/system

These unit files run bash scripts that are located under
~/f26dash/launchscripts

BUILDS
------
This section will cover how to build new updates to the backend/frontend

Updating the backend is as simple as replacing the old python scripts with
new ones. This can easily be done from another machine using secure copy.

Updating the frontend requires the compilation of a node-webkit binary. The
makefile in ~/f26dash/frontend defines this automated build. It first zips 
the required node files, the html file that contains the javascript, the JS 
graphics library,and the RIT racing PNG. This zip file is then concatenated
with the nw binary located in ~/f26dash/frontend/dep/nwjs

ALIAS
-----

Some useful aliases are defined under ~/.bash_profile that shortcut the
following commands:

startf = sudo systemctl start dash_frontend (starts the frontend)
stopf = sudo systemctl stop dash_frontend (stops the frontend)
statf = sudo systemctl status dash_frontend (returns the frontend output)

startb = sudo systemctl start dash_backend (starts the backend)
stopb = sudo systemctl stop dash_backend (stops the backend)
statb = sudo systemctl status dash_backend (returns the backend output)
