import os

class PipeClient:
    def __init__(self, uuid):
        self.fifo_in = f"/tmp/pycppfifobridge_in_{uuid}"
        self.fifo_out = f"/tmp/pycppfifobridge_out_{uuid}"
        self._fifo_in_handle = None
        self._fifo_out_handle = None

    def open(self):
        """Open both FIFOs and keep them open for multiple commands."""
        if not os.path.exists(self.fifo_in):
            raise FileNotFoundError(f"Input FIFO {self.fifo_in} not found")
        if not os.path.exists(self.fifo_out):
            raise FileNotFoundError(f"Output FIFO {self.fifo_out} not found")
        
        # Open the input FIFO in write mode
        # 'w' mode will close the file automatically when the object is de-referenced,
        # but if we keep a handle, we stay open as long as the handle lives.
        self._fifo_in_handle = open(self.fifo_in, "w", buffering=1)  # line-buffered

        # Open the output FIFO in read mode
        self._fifo_out_handle = open(self.fifo_out, "r")

    def close(self):
        """Close both FIFOs. Once closed, no further commands can be sent."""
        if self._fifo_in_handle:
            self._fifo_in_handle.close()
        if self._fifo_out_handle:
            self._fifo_out_handle.close()
        self._fifo_in_handle = None
        self._fifo_out_handle = None

    def run(self, command):
        """Send a single command and read the response, keeping the FIFO open."""
        if not self._fifo_in_handle or not self._fifo_out_handle:
            raise RuntimeError("FIFOs are not open. Call open() before sending commands.")

        # Write the command, followed by a newline
        self._fifo_in_handle.write(command + "\n")
        self._fifo_in_handle.flush()   # ensure the data is actually sent

        # Read back the response (line)
        response = self._fifo_out_handle.readline()
        return response.strip()
