import urwid, subprocess

class interface:

    def __init__(self):
        self.editor = self.make_editor()
        self.swarm = urwid.Pile([self.robot_box(),
                                 self.robot_box(),
                                 self.robot_box()])
        self.output = urwid.Text("output\none\ntwo\n")
        self.col = urwid.Columns([urwid.Pile([urwid.Text("yarn"),                                 
                                         urwid.Columns([self.compile_button(),
                                                        self.switch_button(),
                                                        urwid.Button(u'load'),
                                                        urwid.Button(u'save')]),
                                              urwid.LineBox(self.editor),
                                              urwid.LineBox(self.output)]),
                                  (12,urwid.Pile([urwid.Text("swarm"),self.swarm]))])
        self.top=urwid.Filler(self.col,valign='top')

        f = open("../scm/robot.scm")
        self.src_text = f.read()
        f.close()
        f = open("../compiler/out.asm")
        self.asm_text = f.read()
        f.close()
        self.show_src=True

        

    def make_editor(self):
        edit = urwid.Edit("",multiline=True,wrap='clip')
        urwid.connect_signal(edit, 'change', self.on_editor_changed)
        return edit

    def compile_button(self):
        button = urwid.Button(u'compile')
        urwid.connect_signal(button, 'click', self.on_compile_clicked)
        return button

    def switch_button(self):
        button = urwid.Button(u'lisp/asm')
        urwid.connect_signal(button, 'click', self.on_switch_clicked)
        return button

    def on_compile_clicked(self,button):
        self.src_text = self.editor.get_edit_text()
        f = open("out.scm","w")
        f.write(self.src_text)
        f.close()
        result = subprocess.Popen(['racket', '../compiler/yarnc.scm', 'out.scm'], 
                                  stdout=subprocess.PIPE).communicate()[0]
        self.output.set_text(result)
        f = open("out.asm","r")
        self.asm_text=f.read()
        f.close()


    def on_switch_clicked(self,button):
        if self.show_src:
            self.editor.set_edit_text(self.asm_text)
            self.show_src=False
        else:
            self.editor.set_edit_text(self.src_text)
            self.show_src=True

    def on_editor_changed(self,edit,b):
        pass

    def robot_box(self):
        return urwid.LineBox(urwid.Pile([urwid.Text("robot: 1"),
                                     urwid.Text("pc: 20")]))




i=interface()
urwid.MainLoop(i.top, palette=[('reversed', 'standout', '')]).run()
