import sys

from PySide2.QtWidgets import QApplication
from memory_tester import MemoryTestController


def main():
    app = QApplication(sys.argv)
    memory_test_controller = MemoryTestController()
    memory_test_controller.ui.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
    main()

