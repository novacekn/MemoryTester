from PySide2.QtCore import QThread, Signal
from PySide2 import QtWidgets
from PySide2.QtUiTools import QUiLoader
import psutil

from time import time
import datetime
import os
import subprocess


class MemoryTest(QThread):
    CURRENT_PERCENT = Signal(int)
    CURRENT_TIME = Signal(str)
    CURRENT_TEST = Signal(str)
    CURRENT_PASS = Signal(str)
    CURRENT_PERCENT_STRING = Signal(str)

    def __init__(self):
        super(MemoryTest, self).__init__()
        self.run_time = None
        self.timer = time()
        self.status = None
        self.results = None
        self.requested_bytes = None
        self.memory_tests = None

    def run(self):
        timer = time()
        pass_number = 1

        for test in self.memory_tests:
            proc = subprocess.Popen(['./memory_tester', str(self.requested_bytes), test.lower().replace(' ', '_')], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            pass_string = f'{pass_number}/{len(self.memory_tests)}'
            self.CURRENT_PASS.emit(pass_string)

            while proc.poll() is None:
                self.run_time = time() - self.timer
                line = proc.stdout.readline().decode().strip()
                self.duration = time() - timer
                self.CURRENT_PERCENT.emit(int(line))
                self.CURRENT_PERCENT_STRING.emit(str(line) + '%')
                self.CURRENT_TIME.emit(str(datetime.timedelta(seconds=(time() - timer))))
                # self.CURRENT_TEST.emit(test.replace('_comparison', '').replace('_', ' ').title())
                self.CURRENT_TEST.emit(test)

            if proc.returncode != 0:
                self.status = 'Complete'
                self.results = 'Failed'
                self.run_time = time() - self.timer
                break

            pass_number += 1

        self.status = 'Complete'
        self.results = 'Passed'
        self.run_time = time() - self.timer


class MemoryTestController:
    def __init__(self):
        loader = QUiLoader()
        self.ui = loader.load('memory_tester.ui')

        self._memory_test = MemoryTest()
        self._requested_bytes = None

        self.memory_test_boxes = [
            self.ui.stuck_address,
            self.ui.random_value,
            self.ui.xor_comparison,
            self.ui.sub_comparison,
            self.ui.mul_comparison,
            self.ui.div_comparison,
            self.ui.or_comparison,
            self.ui.and_comparison,
            self.ui.sequential_increment_comparison,
            self.ui.solid_bits_comparison,
            self.ui.checkerboard_comparison,
            self.ui.block_sequential_comparison,
            self.ui.walking_zeroes_comparison,
            self.ui.walking_ones_comparison,
            self.ui.bit_spread_comparison,
            self.ui.bit_flip_comparison,
        ]

        self._free_bytes = self._get_free_memory()
        self._free_megabytes = self._free_bytes >> 20

        self.ui.free_memory_label.setText(str(self._free_megabytes) + ' MB')
        self.ui.test_size_label.setText(str(self._free_megabytes) + ' MB')

        self._memory_test.finished.connect(self._finished)
        self._memory_test.CURRENT_PERCENT.connect(self.ui.test_progress_bar.setValue)
        self._memory_test.CURRENT_TEST.connect(self.ui.test_name_label.setText)
        self._memory_test.CURRENT_PASS.connect(self.ui.tests_completed.setText)
        self._memory_test.CURRENT_PERCENT_STRING.connect(self.ui.test_percent.setText)

        self.ui.run_button.clicked.connect(self._run_tests)
        self.ui.percent_slider.sliderMoved.connect(self._get_requested_bytes)

    def _get_requested_bytes(self):
        self.ui.percent_label.setText(str(self.ui.percent_slider.value()) + '%')
        requested_bytes = int(self._free_bytes * (self.ui.percent_slider.value() / 100))
        self.ui.test_size_label.setText(str(requested_bytes >> 20) + ' MB')
        self._requested_bytes = requested_bytes
        # self._memory_test.requested_bytes = requested_bytes

    def _get_free_memory(self):
        return psutil.virtual_memory().free

    def _run_tests(self):
        tests = []
        for box in self.memory_test_boxes:
            if box.isChecked():
                tests.append(box.text())
                print(box.text().lower().replace(' ', '_'))
        self._memory_test.memory_tests = tests
        self._memory_test.requested_bytes = self._requested_bytes
        self._memory_test.start()

    def _finished(self):
        print('Tests are complete')

