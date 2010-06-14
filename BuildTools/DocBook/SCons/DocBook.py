################################################################################
# DocBook pseudobuilder
################################################################################

import SCons.Util, SCons.Action
import xml.dom.minidom, re, os.path, sys

def generate(env) :
  # Location of stylesheets and catalogs
  docbook_dir = "#/BuildTools/DocBook"
  docbook_xsl_style_dir = env.Dir(docbook_dir + "/Stylesheets").abspath
  docbook_xml_catalog = env.File("catalog.xml").abspath
  if "DOCBOOK_XML_DIR" in env :
    docbook_xml_dir = env.Dir("$DOCBOOK_XML_DIR").abspath
  else :
    docbook_xml_dir = env.Dir("#/3rdParty/DocBook/XML").abspath
  if "DOCBOOK_XSL_DIR" in env :
    docbook_xsl_dir = env.Dir("$DOCBOOK_XSL_DIR").abspath
  else :
    docbook_xsl_dir = env.Dir("#/3rdParty/DocBook/XSL").abspath
  fop_fonts_dir = env.Dir(docbook_dir + "/Fonts").abspath

  # Generates a catalog from paths to external tools
  def buildCatalog(target, source, env) :
    catalog = """<?xml version='1.0'?>
<catalog xmlns="urn:oasis:names:tc:entity:xmlns:xml:catalog" prefer="public">
  <rewriteSystem 
      systemIdStartString="http://www.oasis-open.org/docbook/xml/4.5/" 
      rewritePrefix="%(docbook_xml_dir)s/" />
  <rewriteSystem 
      systemIdStartString="docbook-xsl:/" 
      rewritePrefix="%(docbook_xsl_dir)s/" />
</catalog>"""
    
    docbook_xml_dir = source[0].get_contents()
    docbook_xsl_dir = source[1].get_contents()
    if env["PLATFORM"] == "win32" :
      docbook_xml_dir = docbook_xml_dir.replace("\\","/")
      docbook_xsl_dir = docbook_xsl_dir.replace("\\","/")
    file = open(target[0].abspath, "w")
    file.write(catalog % {
        "docbook_xml_dir" : docbook_xml_dir,
        "docbook_xsl_dir" : docbook_xsl_dir,
      })
    file.close()

  # Generates a FOP config file
  def buildFopConfig(target, source, env) :
    fopcfg = """<fop version=\"1.0\">
  <renderers>
    <renderer mime=\"application/pdf\">
      <fonts>
        <directory recursive=\"true\">%(fonts_dir)s</directory>
      </fonts>
    </renderer>
  </renderers>
</fop>"""

    file = open(target[0].abspath, "w")
    file.write(fopcfg % {
        "fonts_dir" : source[0].get_contents()
      })
    file.close()

  # Builds a DocBook file
  def buildDocBook(env, source) :
    db_env = env.Clone()
    db_env["XMLCATALOGS"] = [docbook_xml_catalog]
    db_env["ENV"].update({"OS" : os.environ.get("OS", "")})

    db_env["XMLLINT"] = env.WhereIs("xmllint")
    db_env["XSLT"] = env.WhereIs("xsltproc")
    db_env["FO"] = env.WhereIs("fop")

    if not db_env["XMLLINT"] or not db_env["XSLT"] :
      return

    # PDF generation
    if db_env["FO"] :
      fo = db_env.XSLT(os.path.splitext(source)[0] + ".fo", source, 
          XSLTSTYLESHEET = db_env["DOCBOOK_XSL_FO"])
      pdf = db_env.FO(fo)

    # HTML generation
    db_env.XSLT(os.path.splitext(source)[0] + ".html", source, 
        XSLTSTYLESHEET = db_env["DOCBOOK_XSL_HTML"])

  # Import tools
  env.Tool("FO", toolpath = [docbook_dir + "/SCons"])
  env.Tool("XSLT", toolpath = [docbook_dir + "/SCons"])

  # Catalog file generation
  env.Command("catalog.xml", [env.Value(docbook_xml_dir), env.Value(docbook_xsl_dir)], SCons.Action.Action(buildCatalog, cmdstr = "$GENCOMSTR"))

  # FO config file generation
  env["FOCFG"] = env.File("fop.cfg").abspath
  env.Command("fop.cfg", [env.Value(fop_fonts_dir)], SCons.Action.Action(buildFopConfig, cmdstr = "$GENCOMSTR"))

  # DocBook stylesheets
  env["DOCBOOK_XSL_FO"] = docbook_xsl_style_dir + "/fo/docbook.xsl"
  env["DOCBOOK_XSL_HTML"] = docbook_xsl_style_dir + "/html/docbook.xsl"
  env.AddMethod(buildDocBook, "DocBook")
      
def exists(env) :
  return True
