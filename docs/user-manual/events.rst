.. _events:

Events
======

When using LwRB in the application, it may be useful to get notification on different events,
such as info when something has been written or read to/from buffer.

Library has support for events that get called each time there has been a modification
in the buffer data, that means on every read or write operation.

Some use cases:

* Notify application layer that LwRB operation has been executed and send debug message
* Unlock semaphore when sufficient amount of bytes have been written/read from/to buffer when application uses operating system
* Write notification to message queue at operating system level to wakeup another task

.. note:: Every operation that modifies ``read`` or ``write`` internal pointers,
        is considered as read or write operation. An exception is *reset* event that sets
        both internal pointers to ``0``

Events are sent by the following functions:

* ``LWRB_EVT_WRITE``: ``lwrb_write``, ``lwrb_write_ex``, ``lwrb_advance``
* ``LWRB_EVT_READ``: ``lwrb_read``, ``lwrb_read_ex``, ``lwrb_skip``
* ``LWRB_EVT_RESET``: ``lwrb_reset``
* ``lwrb_peek`` and ``lwrb_find`` do not send any event

Event is sent only when pointer has actually moved. Operations that read, write, skip or advance ``0`` bytes
(for instance because buffer is empty or full) do not send any event.

.. warning::
    Callback is called from the context of the function that triggered the event.
    ``LWRB_EVT_WRITE`` is called from write context, ``LWRB_EVT_READ`` from read context.
    When buffer is used from ``2`` threads (or thread and interrupt), both can be called at the same time,
    hence callback implementation must be safe for concurrent execution. See :ref:`thread_safety`.

.. literalinclude:: ../examples_src/example_events.c
    :language: c
    :linenos:
    :caption: Example code for events

