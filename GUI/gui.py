import numpy as np
import numpy.ctypeslib as ctl
import ctypes
from ctypes import *
import tkinter as tk
import tkinter.messagebox
from tkinter import filedialog
from tkinter import ttk
from functools import partial

libname = 'libznfs_lib_4.so'
libdir = '/home/se/software_class/Tar_Untar_cpp/client_so_lib/cmake-build-debug'
lib = ctl.load_library(libname, libdir)


localPackUnpack = lib.localPackUnpack
localPackUnpack.argtypes = [ctypes.c_wchar_p, ctypes.c_wchar_p]
localPackUnpack.restype = ctypes.c_int

localCompareDiff = lib.localCompareDiff
localCompareDiff.argtypes = [ctypes.c_wchar_p, ctypes.c_wchar_p]
localCompareDiff.restype = ctypes.c_wchar_p

logInCPP = lib.logIn
logInCPP.argtypes = [ctypes.c_wchar_p]
logInCPP.restype = ctypes.c_int

upLoadCPP = lib.upLoad
upLoadCPP.argtypes = [ctypes.c_wchar_p, ctypes.c_wchar_p, ctypes.c_wchar_p]
upLoadCPP.restype = ctypes.c_int

showAllCPP = lib.showAll
showAllCPP.argtypes = [ctypes.c_wchar_p]
showAllCPP.restype = ctypes.c_wchar_p

downLoadCPP = lib.downLoad
downLoadCPP.argtypes = [ctypes.c_wchar_p, ctypes.c_wchar_p, ctypes.c_wchar_p, ctypes.c_wchar_p]
downLoadCPP.restype = ctypes.c_int

def getPath(type='source', master=None, file_type=None, lb=None):
    if type == 'source':
        if file_type == 'dir':
            path = filedialog.askdirectory(title='Select {} dir'.format(type),
                                           parent=master,
                                           initialdir='/home')
        elif file_type == 'file':
            path = filedialog.askopenfilenames(title='Select {} file'.format(type),
                                               parent=master,
                                               initialdir='/home')

    else:
        if file_type is None or file_type == 'dir':
            path = filedialog.askdirectory(title='Select {} dir'.format(type),
                                           parent=master,
                                           initialdir='/home')
        elif file_type == 'file':
            path = filedialog.askopenfilenames(title='Select {} file'.format(type),
                                               parent=master,
                                               initialdir='/home')

    if path != '':
        lb.config(text=path)


class localTrans:
    def __init__(self, master=None):
        self.master = master

    def backup(self, src_path, tgt_path):
        if localPackUnpack(src_path, tgt_path) == 0:
            tk.messagebox.showinfo(title='Result', message='Backup done!')
        else:
            tk.messagebox.showerror(title='Error', message='Fail to backup!')

    def createLocalBackupWindow(self):
        self.local_win = tk.Toplevel(self.master)
        self.local_win.title('Local Backup/Recover')
        self.local_win.geometry('500x510+400+200')

        self.local_win.grab_set()  # this forces all focus on the top level until Toplevel is closed

        # Create Checkbuttons
        radio_var = tk.StringVar(self.local_win, value='dir')

        src_select_dir = tk.Radiobutton(self.local_win, text='directory', variable=radio_var, value='dir')
        src_select_file = tk.Radiobutton(self.local_win, text='file', variable=radio_var, value='file')

        src_select_dir.grid(row=1, column=0, sticky='E')
        src_select_file.grid(row=1, column=1, sticky='E')

        separator = ttk.Separator(self.local_win, orient='horizontal')
        separator.grid(row=2, columnspan=10, sticky="ew")

        # global src_lb, tgt_lb
        src_lb = tk.Label(self.local_win, text='')
        src_lb.grid(row=0, column=1)

        tgt_lb = tk.Label(self.local_win, text='')
        tgt_lb.grid(row=3, column=1)

        # Create Input Buttons
        src_btn = tk.Button(self.local_win, text='Source Path', command=lambda: getPath('source', self.local_win,
                                                                                        radio_var.get(), src_lb))
        src_btn.grid(row=0, column=0)

        tgt_btn = tk.Button(self.local_win, text='Target Path', command=lambda: getPath('target', self.local_win,
                                                                                        None, tgt_lb))
        tgt_btn.grid(row=3, column=0)

        # Create Backup Buttons
        backup_btn = tk.Button(self.local_win, text='Backup', command=lambda: self.backup(src_lb.cget('text'),
                                                                                          tgt_lb.cget('text')))
        backup_btn.grid(row=4, column=0)



class diffTrans:
    def __init__(self, master):
        self.master = master

    def compareDiff(self, src_path, tgt_path, src_listbox, tgt_listbox, src_same_listbox, tgt_same_listbox):
        # TODO: Later call cpp
        src_listbox.delete(0, tk.END)
        tgt_listbox.delete(0, tk.END)
        src_same_listbox.delete(0, tk.END)
        tgt_same_listbox.delete(0, tk.END)

        diff_str = localCompareDiff(src_path, tgt_path)  # error occurs here, cannot pass this sentence

        diff_list = diff_str.split('[SEP]')     # 4 sections

        src_diff = diff_list[0].strip().split('\n')
        for i in range(len(src_diff)):
            if src_diff[i] != '[EMP]':
                src_listbox.insert(tk.END, src_diff[i])

        tgt_diff = diff_list[1].strip().split('\n')
        for i in range(len(tgt_diff)):
            if tgt_diff[i] != '[EMP]':
                tgt_listbox.insert(tk.END, tgt_diff[i])

        src_sa_diff = diff_list[2].strip().split('\n')
        for i in range(len(src_sa_diff)):
            if src_sa_diff[i] != '[EMP]':
                src_same_listbox.insert(tk.END, src_sa_diff[i])

        tgt_sa_diff = diff_list[3].strip().split('\n')
        for i in range(len(tgt_sa_diff)):
            if tgt_sa_diff[i] != '[EMP]':
                tgt_same_listbox.insert(tk.END, tgt_sa_diff[i])


    def createDompareWindow(self):
        self.compare_win = tk.Toplevel(master=self.master)
        self.compare_win.title('Compare Difference')
        self.compare_win.geometry('500x510+400+200')

        self.compare_win.grab_set()  # this forces all focus on the top level until Toplevel is closed

        # Create Checkbuttons
        radio_var_s = tk.StringVar(self.compare_win, value='dir')

        src_select_dir = tk.Radiobutton(self.compare_win, text='directory', variable=radio_var_s, value='dir')
        src_select_file = tk.Radiobutton(self.compare_win, text='file', variable=radio_var_s, value='file')

        src_select_dir.grid(row=1, column=0)
        src_select_file.grid(row=1, column=1)

        radio_var_t = tk.StringVar(self.compare_win, value='dir')

        tgt_select_dir = tk.Radiobutton(self.compare_win, text='directory', variable=radio_var_t, value='dir')
        tgt_select_file = tk.Radiobutton(self.compare_win, text='file', variable=radio_var_t, value='file')

        tgt_select_dir.grid(row=4, column=0)
        tgt_select_file.grid(row=4, column=1)

        separator = ttk.Separator(self.compare_win, orient='horizontal')
        separator.grid(row=2, columnspan=10, sticky="ew")

        # global src_lb, tgt_lb
        src_lb = tk.Label(self.compare_win, text='')
        src_lb.grid(row=0, column=1, columnspan=3)

        tgt_lb = tk.Label(self.compare_win, text='')
        tgt_lb.grid(row=3, column=1, columnspan=3)

        # Create Input Buttons
        src_btn = tk.Button(self.compare_win, text='Source Path', command=lambda: getPath('source', self.compare_win,
                                                                                          radio_var_s.get(), src_lb))
        src_btn.grid(row=0, column=0)

        tgt_btn = tk.Button(self.compare_win, text='Target Path', command=lambda: getPath('target', self.compare_win,
                                                                                          radio_var_t.get(), tgt_lb))
        tgt_btn.grid(row=3, column=0)

        # Create Compare Button
        compare_btn = tk.Button(self.compare_win, text='Compare', command=lambda: self.compareDiff(src_lb.cget('text'),
                                                                                                   tgt_lb.cget('text'),
                                                                                                   src_listbox,
                                                                                                   tgt_listbox,
                                                                                                   src_same_listbox,
                                                                                                   tgt_same_listbox))
        compare_btn.grid(row=5, column=0)

        # Create Diff Label
        src_diff_lb = tk.Label(self.compare_win, text='Source')
        src_diff_lb.grid(row=6, column=0)

        tgt_diff_lb = tk.Label(self.compare_win, text='Target')
        tgt_diff_lb.grid(row=6, column=3)

        # Create Vertical Scrollbar
        src_scrollbar_v = tk.Scrollbar(self.compare_win, orient=tk.VERTICAL)
        src_scrollbar_v.grid(row=7, column=2, rowspan=5, sticky='W' + 'N' + 'S')

        tgt_scrollbar_v = tk.Scrollbar(self.compare_win, orient=tk.VERTICAL)
        tgt_scrollbar_v.grid(row=7, column=5, rowspan=5, sticky='W' + 'N' + 'S')

        # Create Horizontal Scrollbar
        src_scrollbar_h = tk.Scrollbar(self.compare_win, orient=tk.HORIZONTAL)
        src_scrollbar_h.grid(row=12, column=0, columnspan=2, sticky='N' + 'W' + 'E')

        tgt_scrollbar_h = tk.Scrollbar(self.compare_win, orient=tk.HORIZONTAL)
        tgt_scrollbar_h.grid(row=12, column=3, columnspan=2, sticky='N' + 'W' + 'E')

        #
        # Create Vertical Scrollbar
        src_scrollbar_v_2 = tk.Scrollbar(self.compare_win, orient=tk.VERTICAL)
        src_scrollbar_v_2.grid(row=14, column=2, rowspan=5, sticky='W' + 'N' + 'S')

        tgt_scrollbar_v_2 = tk.Scrollbar(self.compare_win, orient=tk.VERTICAL)
        tgt_scrollbar_v_2.grid(row=14, column=5, rowspan=5, sticky='W' + 'N' + 'S')

        # Create Horizontal Scrollbar
        src_scrollbar_h_2 = tk.Scrollbar(self.compare_win, orient=tk.HORIZONTAL)
        src_scrollbar_h_2.grid(row=19, column=0, columnspan=2, sticky='N' + 'W' + 'E')

        tgt_scrollbar_h_2 = tk.Scrollbar(self.compare_win, orient=tk.HORIZONTAL)
        tgt_scrollbar_h_2.grid(row=19, column=3, columnspan=2, sticky='N' + 'W' + 'E')

        # Create list for S, T
        # Create Listbox
        src_listbox = tk.Listbox(self.compare_win, yscrollcommand=src_scrollbar_v.set, xscrollcommand=src_scrollbar_h.set)
        src_listbox.grid(row=7, column=0, rowspan=5, columnspan=2)

        tgt_listbox = tk.Listbox(self.compare_win, yscrollcommand=tgt_scrollbar_v.set, xscrollcommand=tgt_scrollbar_h.set)
        tgt_listbox.grid(row=7, column=3, rowspan=5, columnspan=2)

        #
        s_diff_lb_2 = tk.Label(self.compare_win, text='S diff content')
        s_diff_lb_2.grid(row=13, column=0)

        tgt_diff_lb_2 = tk.Label(self.compare_win, text='T diff content')
        tgt_diff_lb_2.grid(row=13, column=3)

        src_same_listbox = tk.Listbox(self.compare_win, yscrollcommand=src_scrollbar_v_2.set,
                                      xscrollcommand=src_scrollbar_h_2.set)
        src_same_listbox.grid(row=14, column=0, rowspan=5, columnspan=2)

        tgt_same_listbox = tk.Listbox(self.compare_win, yscrollcommand=tgt_scrollbar_v_2.set,
                                      xscrollcommand=tgt_scrollbar_h_2.set)
        tgt_same_listbox.grid(row=14, column=3, rowspan=5, columnspan=2)

        # Attach Listbox to Scrollbar
        src_scrollbar_v.config(command=src_listbox.yview)
        tgt_scrollbar_v.config(command=tgt_listbox.yview)

        src_scrollbar_h.config(command=src_listbox.xview)
        tgt_scrollbar_h.config(command=tgt_listbox.xview)

        #
        src_scrollbar_v_2.config(command=src_same_listbox.yview)
        tgt_scrollbar_v_2.config(command=tgt_same_listbox.yview)

        src_scrollbar_h_2.config(command=src_same_listbox.xview)
        tgt_scrollbar_h_2.config(command=tgt_same_listbox.xview)




class NetdiskTrans:
    def __init__(self, master=None):
        self.master = master
        self.key = None

    def checkNumber(self, password):
        if len(password) > 8:
            return False
        for i in range(len(password)):
            if password[i] < '0' or password[i] > '9':
                return False

        return True

    def checkPassword(self, password):
        if password != '':
            valid = self.checkNumber(password)
            if valid:

                # TODO: Later call .so here to check if password is valid on server
                valid_pass = logInCPP(password)
                if valid_pass == 0:
                    self.password = password
                    self.password_win.destroy()
                    self.createNetdiskWindow()
                else:
                    tk.messagebox.showerror(title='Error', message='Wrong password!')

            else:
                tk.messagebox.showerror(title='Error', message='Invalid password!')

    def returnKey(self, key, input_key_win, path, type, name_listbox=None):
        if key != '':
            valid_pass = self.checkNumber(key)
            if valid_pass:
                self.key = key
                #self.input_key_win.destroy()
                input_key_win.destroy()
                if type == 'up':
                    self.upLoad2(path)
                else:
                    self.downLoad2(path, name_listbox)
            else:
                tk.messagebox.showerror(title='Error', message='Invalid key!')



    def createInputKeyWindow(self, master, path, type, name_listbox=None):
        self.input_key_win = tk.Toplevel(master=master)
        self.input_key_win.title('Key')
        self.input_key_win.geometry('250x100+400+200')

        self.input_key_win.grab_set()

        key_lb = tk.Label(self.input_key_win, text='Key:')
        key_lb.grid(row=0, column=0)

        key_en = tk.Entry(self.input_key_win)
        key_en.grid(row=0, column=1)

        key_confirm_btn = tk.Button(self.input_key_win, text='Confirm',
                                         command=lambda: self.returnKey(key_en.get(), self.input_key_win, path, type,
                                                                        name_listbox))
        key_confirm_btn.grid(row=1, column=0, columnspan=2)


    def showALL(self, name_listbox):
        name_listbox.delete(0, tk.END)

        # TODO: Later should click button 'Show All' to display all files under this password on server
        server_file = showAllCPP(self.password)
        if server_file == 'Fail':
            tk.messagebox.showerror(title='Error', message='Some Error Occurs!')
        else:
            server_file_list = server_file.strip().split('\n')
            for i in range(len(server_file_list)):
                name_listbox.insert(tk.END, server_file_list[i])

    def upLoad(self, upload_path, master):
        # TODO
        if upload_path != '':
            self.createInputKeyWindow(master, upload_path, 'up')
            """
            if not self.key is None:
                valid_up = upLoadCPP(upload_path, self.password, self.key)
                self.key = None

                if valid_up == 0:
                    tk.messagebox.showinfo(title='Success', message='Upload successfully!')
                else:
                    tk.messagebox.showerror(title='Error', message='Fail to upload!')
            """

    def upLoad2(self, upload_path):
        if not self.key is None:
            valid_up = upLoadCPP(upload_path, self.password, self.key)
            self.key = None

            if valid_up == 0:
                tk.messagebox.showinfo(title='Success', message='Upload successfully!')
            else:
                tk.messagebox.showerror(title='Error', message='Fail to upload!')


    def downLoad(self, download_path, master, name_listbox):
        if download_path != '':
            self.createInputKeyWindow(master, download_path, 'down', name_listbox)

    def downLoad2(self, download_path, name_listbox):
        # TODO
        if not self.key is None:
            try:
                download_name = name_listbox.get(name_listbox.curselection())
                valid = downLoadCPP(download_name, download_path, self.password, self.key)
                self.key = None

                if valid == 0:
                    tk.messagebox.showinfo(title='Success', message='Download successfully!')
                else:
                    tk.messagebox.showerror(title='Error', message='Fail to download!')

            except tk.TclError:
                tk.messagebox.showinfo(title='Hint', message='Need to select a file on server')

    def createPasswordWindow(self):
        self.password_win = tk.Toplevel(master=self.master)
        self.password_win.title('Password')
        self.password_win.geometry('250x100+400+200')
        self.password_win.grab_set()

        password_lb = tk.Label(self.password_win, text='Password:')
        password_lb.grid(row=0,column=0)

        password_en = tk.Entry(self.password_win)
        password_en.grid(row=0,column=1)

        password_confirm_btn = tk.Button(self.password_win, text='Confirm',
                                         command=lambda: self.checkPassword(password_en.get()))
        password_confirm_btn.grid(row=1,column=0,columnspan=2)

    def createNetdiskWindow(self):
        self.netdisk_win = tk.Toplevel(master=self.master)
        self.netdisk_win.title('Netdisk')
        self.netdisk_win.geometry('450x400+400+200')
        self.netdisk_win.grab_set()

        # Create Checkbuttons
        radio_var = tk.StringVar(self.netdisk_win, value='dir')

        upload_select_dir = tk.Radiobutton(self.netdisk_win, text='directory', variable=radio_var, value='dir')
        upload_select_file = tk.Radiobutton(self.netdisk_win, text='file', variable=radio_var, value='file')

        upload_select_dir.grid(row=1, column=0, sticky='E')
        upload_select_file.grid(row=1, column=1, sticky='E')

        separator = ttk.Separator(self.netdisk_win, orient='horizontal')
        separator.grid(row=2, columnspan=10, sticky="ew")

        # global src_lb, tgt_lb
        upload_lb = tk.Label(self.netdisk_win, text='')
        upload_lb.grid(row=0, column=1)

        download_lb = tk.Label(self.netdisk_win, text='')
        download_lb.grid(row=3, column=1)

        # Create Input Buttons
        upload_path_btn = tk.Button(self.netdisk_win, text='Upload from',
                            command=lambda: getPath('source', self.netdisk_win, radio_var.get(), upload_lb))
        upload_path_btn.grid(row=0, column=0)

        download_path_btn = tk.Button(self.netdisk_win, text='Download to',
                                 command=lambda: getPath('target', self.netdisk_win, None, download_lb))
        download_path_btn.grid(row=3, column=0)

        # Create Scrollbar
        scrollbar = tk.Scrollbar(self.netdisk_win, orient=tk.VERTICAL)
        scrollbar.grid(row=4, column=2, rowspan=5, sticky='W' + 'N' + 'S')

        # Create Listbox
        name_listbox = tk.Listbox(self.netdisk_win, yscrollcommand=scrollbar.set)
        name_listbox.grid(row=4, column=1, rowspan=5, padx=10)

        # Attach Listbox to Scrollbar
        scrollbar.config(command=name_listbox.yview)

        # Create Show All Button
        show_all_btn = tk.Button(self.netdisk_win, text='Show All', command=lambda :self.showALL(name_listbox))
        show_all_btn.grid(row=10, column=1)

        # Create Upload/Download Buttons
        upload_btn = tk.Button(self.netdisk_win, text='Upload',
                               command=lambda: self.upLoad(upload_lb.cget('text'), self.netdisk_win))
        upload_btn.grid(row=10, column=0)
        download_btn = tk.Button(self.netdisk_win, text='Download',
                                 command=lambda: self.downLoad(download_lb.cget('text'), self.netdisk_win, name_listbox))
        download_btn.grid(row=10, column=2)


class BackUp:
    def __init__(self, master=None):
        self.master = master
        self.window = None

    def createLocalBackupWindow(self):
        self.window = localTrans(self.master)
        self.window.createLocalBackupWindow()

    def createCompareWindow(self):
        self.window = diffTrans(self.master)
        self.window.createDompareWindow()

    def createNetdiskWindow(self):
        self.window = NetdiskTrans(self.master)
        self.window.createPasswordWindow()

if __name__ == '__main__':
    root = tk.Tk()
    root.geometry('450x400+400+200')
    root.title('Backup/Recovery GUI')

    my_backup_win = BackUp(root)

    #Create Function Option Buttons
    local_func = tk.Button(root, text='Local Backup/Recover', command=my_backup_win.createLocalBackupWindow)
    local_func.place(x=120,y=80,width=180,height=25)

    diff_func = tk.Button(root, text='Compare Difference', command=my_backup_win.createCompareWindow)
    diff_func.place(x=120,y=180,width=180,height=25)

    net_func = tk.Button(root, text='Net Disk Backup/Recover', command=my_backup_win.createNetdiskWindow)
    net_func.place(x=120,y=280,width=180,height=25)

    root.mainloop()
