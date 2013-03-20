import random
class PageAccount():
    def serveAccount(self, qs, isSession = False):
        if not isSession:
            self.setSession(qs)
        if not self.getLevel() > 0:
            self.serveDefault(qs, True)
            return
        changes = {}
        webAttributes = self.settings.getCfgValue("userWebAttributes")
        tses = self.sessions.get_session(qs["sma"][0])
        changes["INFO"] = ''
        if "saveuser" in qs:
                #save changes
                if qs["saveuser"][0] == tses.to_md5("randtextsave") and self.isSafe():
                    user = self.users.get_user("name", tses._user["name"])
                    if user:
                        username = user["name"]
                        for atr in webAttributes:
                            if not atr in qs:
                                continue
                            if atr in ["name", "level"]:
                                continue
                            self.users.set_attribute(username, atr, tses.from_code(qs[atr][0]))
                        if "password" in qs and "pswcheck" in qs:
                            
                            if qs["pswcheck"][0] == tses.to_md5(tses.from_code(qs["password"][0])):
                                self.users.set_attribute(username, "password", tses.from_code(qs["password"][0]))
                            else:
                                print("Nesakrit!!")
                        changes["INFO"] = "<h4 class='suc'> Saved changes! </h4>"
                        self.users.write_in_file()
                    else:
                        changes["INFO"] = "<h4 class='err'> Could not save changes! </h4>"
                else:
                    changes["INFO"] = "<h4 class='err'> Wrong data for saving! </h4>"
        self.send_response(200)
        self.sendDefaultHeaders()
        self.end_headers()
        self.serveHeader("account", qs)
        changes["FORM"] = ''
        if self.isSafe():
            if tses:
                tuser=tses._user
                formCode = "<form>"
                formCode += "<p> Name: <strong>" + tses.to_code(str(tses._user["name"])) + "</strong>"
                for atr in webAttributes:
                    tcod = str(random.randint(10000000, 99999999))
                    if atr in ["name", "password"]:
                        continue
                    else:
                        formCode += "<p>" + atr + ": " + "<input autocomplete='off' type='text' class='coded tocode' id='" + tcod
                        formCode += "' value=\"" + tses.to_code(tuser.get(atr, ""), False, tcod) + "\" name='" + atr + "'></p>"
                formCode += "<br><p>Password: <input autocomplete='off' type='password' class='tocode' id='psw1' name='password'></p>"
                formCode += "<p><input type='hidden' id='pswcheck' name='pswcheck'>"
                formCode += "Re password: <input autocomplete='off' type='password' class='tocode' id='psw2'></p>"
                formCode += "<p><input type='hidden' class='md5' id='randtextsave' name='saveuser'>"
                formCode += "<input type='submit' onclick='return userSave()' value='Save'></p>"
                formCode += "</form>"
                changes["FORM"] = formCode

        self.serveBody("account", qs, changes)
        self.serveFooter()

