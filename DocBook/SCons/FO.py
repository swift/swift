import SCons.Util
import xml.dom.minidom, re

################################################################################
# XSL-FO builder
################################################################################

def generate(env) :
  def generate_actions(source, target, env, for_signature) :
    if len(env["FOCFG"]) > 0 :
      cmd = "$FO -c $FOCFG $FOFLAGS $SOURCE $TARGET"
    else :
      cmd = "$FO $FOFLAGS $SOURCE $TARGET"
    return SCons.Action.Action(cmd, cmdstr = "$FOCOMSTR")

  def modify_sources(target, source, env) :
    if len(env["FOCFG"]) > 0 :
      source.append(env["FOCFG"])
    return target, source

  def scan_fo(node, env, path) :
    dependencies = set()
    try :
      document = xml.dom.minidom.parseString(node.get_contents())
    except xml.parsers.expat.ExpatError:
      return []
    for include in document.getElementsByTagNameNS("http://www.w3.org/1999/XSL/Format", "external-graphic") :
      m = re.match("url\((.*)\)", include.getAttribute("src"))
      if m :
        dependencies.add(m.group(1))
    return list(dependencies)

  env["FO"] = "fop"
  env["FOFLAGS"] = ""
  env["FOCFG"] = ""
  env["BUILDERS"]["FO"] = SCons.Builder.Builder(
        generator = generate_actions,
        emitter = modify_sources,
        source_scanner = SCons.Scanner.Scanner(function = scan_fo, skeys = [".fo"]),
        suffix = ".pdf",
        src_suffix = ".fo"
      )

def exists(env) :
  return True
