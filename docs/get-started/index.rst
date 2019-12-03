Get started
===========

.. _download_library:

Download library
^^^^^^^^^^^^^^^^

Library is primarly hosted on `Github <https://github.com/MaJerle/ringbuff>`_.

* Download latest release from `releases area <https://github.com/MaJerle/ringbuff/releases>`_ on Github
* Clone `develop` branch for latest development

Download from releases
**********************

All releases are available on Github releases `releases area <https://github.com/MaJerle/ringbuff/releases>`_.

Clone from Github
*****************

First-time clone
""""""""""""""""

* Download and install ``git`` if not already
* Open console and navigate to path in the system to clone repository to. Use command ``cd your_path``
* Run ``git clone --recurse-submodules https://github.com/MaJerle/ringbuff`` command to clone repository including submodules or
* Run ``git clone --recurse-submodules --branch develop https://github.com/MaJerle/ringbuff`` to clone `development` branch
* Navigate to ``examples`` directory and run favourite example

Update cloned to latest version
"""""""""""""""""""""""""""""""

* Open console and navigate to path in the system where your resources repository is. Use command ``cd your_path``
* Run ``git pull origin master --recurse-submodules`` command to pull latest changes and to fetch latest changes from submodules
* Run ``git submodule foreach git pull origin master`` to update & merge all submodules

Add library to project
^^^^^^^^^^^^^^^^^^^^^^

At this point it is assumed that you have successfully download library, either cloned it or from releases page.

* Copy ``ringbuff`` folder to your project
* Add ``ringbuff/src/include`` folder to `include path` of your toolchain
* Add source files from ``ringbuff/src/`` folder to toolchain build
* Build the project

Minimal example code
^^^^^^^^^^^^^^^^^^^^

Run below example to test and verify library

.. code-block:: c

    /* Buffer variables */
    ringbuff_t buff;                            /* Declare ring buffer structure */
    uint8_t buff_data[8];                       /* Declare raw buffer data array */

    /* Application variables
    uint8_t data[2];                            /* Application working data */

    /* Application code ... */
    ringbuff_init(&buff, buff_data, sizeof(buff_data)); /* Initialize buffer */

    /* Write 4 bytes of data */
    ringbuff_write(&buff, "0123", 4);

    /* Print number of bytes in buffer */
    printf("Bytes in buffer: %d\r\n", (int)ringbuff_get_full(&buff));

    /* Will print "4" */