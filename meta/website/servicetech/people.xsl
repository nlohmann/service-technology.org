<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
  <html>
  <body>
  <h2>People</h2>
    <table border="1">
      <xsl:for-each select="people/person">
      <tr>
        <td><a><xsl:attribute name="href"><xsl:value-of select="link"/></xsl:attribute><xsl:value-of select="name"/></a></td>
        <td><xsl:value-of select="position"/></td>
        <td><xsl:value-of select="topic"/></td>
       </tr>
      </xsl:for-each>
    </table>
  </body>
  </html>
</xsl:template>
</xsl:stylesheet>

