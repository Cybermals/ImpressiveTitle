"""Data File Encryptor

A tool for encrypting the data files using a modern encryption algorithm.
"""
import base64
import os
from pathlib import Path

from cryptography.hazmat.primitives.ciphers.aead import ChaCha20Poly1305
from kivy.app import App
from kivy.properties import ListProperty, ObjectProperty, StringProperty
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.relativelayout import RelativeLayout


# Classes
# =======
class FileDialog(BoxLayout):
    path = StringProperty()
    filters = ListProperty()
    open_callback = ObjectProperty()

    def open(self):
        # Return if no selection
        if not len(self.ids.file_view.selection):
            return
        
        # Pass first item from selection to open callback and hide this dialog
        self.open_callback(Path(self.ids.file_view.path) / Path(self.ids.file_view.selection[0]))
        self.parent.remove_widget(self)

    def cancel(self):
        self.parent.remove_widget(self)


class MainScreen(RelativeLayout):
    infile = StringProperty()
    outfile = StringProperty()
    key = StringProperty()
    nonce = StringProperty()
    file_dialog = ObjectProperty()

    def set_infile(self, path):
        self.ids.infile.text = str(path)
        self.ids.infile.cursor = (0, 0)
        self.ids.outfile.text = str(path.name).replace("-unencrypted", "")
        self.ids.outfile.cursor = (0, 0)

    def choose_infile(self):
        # Show file dialog
        if not self.file_dialog:
            self.file_dialog = FileDialog()

        self.file_dialog.path = str(Path.cwd())
        self.file_dialog.filters = ["*.dat"]
        self.file_dialog.open_callback = self.set_infile
        self.add_widget(self.file_dialog)

    def generate_key(self):
        # Generate a new encryption key and nonce
        self.ids.key.text = base64.encodebytes(ChaCha20Poly1305.generate_key())
        self.ids.key.cursor = (0, 0)
        self.ids.nonce.text = base64.encodebytes(os.urandom(12))
        self.ids.nonce.cursor = (0, 0)

    def encrypt_file(self):
        # Load input file
        with open(self.infile, "rb") as f:
            data = f.read()

        # Encrypt file
        chacha = ChaCha20Poly1305(base64.decodebytes(self.key.encode()))
        path = Path(self.infile).parent / self.outfile

        with path.open("wb") as f:
            f.write(chacha.encrypt(
                base64.decodebytes(self.nonce.encode()),
                data,
                None
            ))


class DataFileEncryptor(App):
    def build(self):
        return MainScreen()
    

# Entrypoint
if __name__ == "__main__":
    DataFileEncryptor().run()
