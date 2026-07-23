from .SerialCOM import BaseCom, list_ports
from .FakeCOM import FakeCom

__all__ = ["BaseCom", "FakeCom", "list_ports"]
