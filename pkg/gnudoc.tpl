[= AutoGen5 Template -*- Mode: html -*-

html=autogen.html

# Time-stamp: "2004-10-12 20:12:29 bkorb"
# Version:    "$Revision: 3.5 $

# AutoGen Options copyright 1992-2004 Bruce Korb

=]
<?xml version="1.0" encoding="utf-8" ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN"
    "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
[=(dne "  ==  " "<!-- ")=]

  ==  $Id: gnudoc.tpl,v 3.5 2004/10/13 03:13:02 bkorb Exp $

  ***  THEREFORE  *** if you make changes to this file, please
  email the author so it will not be overwritten  :-) "

  --><html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en">
<head>
  <title>AutoGen - The Automated Program Generator</title>
  <meta http-equiv="content-type" content='text/html; charset=utf-8' />
  <link rel="stylesheet" type="text/css" href="/gnu.css" />
  <link rev="made" href="webmasters@gnu.org" />
  <meta name="generator" content="AutoGen [=(. autogen-version)=]">
</head>

<!-- This document is in XML, and xhtml 1.0 -->
<!-- Please make sure to properly nest your tags -->
<!-- and ensure that your final document validates -->
<!-- consistent with W3C xhtml 1.0 and CSS standards -->
<!-- See validator.w3.org -->

<body bgcolor="#FFFFFF" text="#000000" link="#1F00FF" alink="#FF0000"
      vlink="#9900DD">
<h3>AutoGen - Table of Contents</H3>

<address>Free Software Foundation</address>
<address>last updated [=`date '+%B %e, %Y'`=]</address>
<p>
<a href="/graphics/gnu-head.jpg">
        <img src="/graphics/gnu-head-sm.jpg"
        alt=" [image of the head of a GNU] "
        width="129" height="122" />
</a>
<a href="/philosophy/gif.html">(no gifs due to patent problems)</a>
</p>
<hr />

<p>This manual (autogen) is available in the following formats:</p>[=

(define (compute-size f)
  (begin
     (define fsiz (if (access? f R_OK)  (stat:size (stat f)) 0 ))
     (if (< fsiz 4096)
         (set! fsiz (number->string fsiz))
         (if (< fsiz (* 2 1024 1024))
             (set! fsiz (sprintf "%dK"  (inexact->exact (/ fsiz 1024))))
             (set! fsiz (sprintf "%dMB" (inexact->exact (/ fsiz 1048576))))
     )   )
     fsiz
) )

=]
<ul>
  <li>formatted in <a href="html_mono/autogen.html">HTML ([=
      (compute-size "html_mono/autogen.html")
      =] characters)</a> entirely on one web page.
  <li>formatted in <a href="html_mono/autogen.html.gz">HTML ([=
      (compute-size "html_mono/autogen.html.gz")
      =] gzipped bytes)</a> entirely on one web page.
  <li> formatted in <a href="html_chapter/autogen_toc.html">HTML</a>
       with one web page per chapter.
  <li> formatted in <a href="html_chapter/autogen_chapter_html.tar.gz">HTML ([=
      (compute-size "html_chapter/autogen_chapter_html.tar.gz")
      =] gzipped tar file)</a> with one web page per chapter.
  <li> formatted in <a href="html_node/autogen_toc.html">HTML</a>
       with one web page per node.
  <li> formatted in <a href="html_node/autogen_node_html.tar.gz">HTML ([=
      (compute-size "html_node/autogen_node_html.tar.gz")
      =] gzipped tar file)</a> with one web page per node.
  <li>formatted as an <a href="info/autogen.info.gz">Info document ([=
      (compute-size "info/autogen.info.gz")
      =] bytes gzipped tar file)</a>.
  <li>formatted as <a href="text/autogen.txt">ASCII text ([=
      (compute-size "text/autogen.txt")
      =] characters)</a>.
  <li>formatted as <a href="text/autogen.txt.gz">ASCII text ([=
      (compute-size "text/autogen.txt.gz")
      =] gzipped bytes)</a>.
  <li>formatted as <a href="dvi/autogen.dvi.gz">a TeX dvi file ([=
      (compute-size "dvi/autogen.dvi.gz")
      =] gzipped bytes)</a>.
  <li>formatted as <a href="pdf/autogen.pdf.gz">a PDF file ([=
      (compute-size "pdf/autogen.pdf.gz")
      =] gzipped bytes)</a>.
  <li>formatted as <a href="ps/autogen.ps.gz">a PostScript file ([=
      (compute-size "ps/autogen.ps.gz")
      =] gzipped bytes)</a>.
  <li>the original <a href="texi/autogen.texi.gz">Texinfo source ([=
      (compute-size "texi/autogen.texi.gz")
      =] gzipped bytes)</a>
</ul>

<p>(This page generated by the <a href="http://www.gnu.org/software/autogen"
>autogen program</a> in conjunction with <a
href="http://savannah.gnu.org/cgi-bin/viewcvs/autogen/autogen/pkg/gnudoc.tpl"
>a very simple template</a>.)</p>

<div class="copyright">
<p>
Return to the <a href="/home.html">GNU Project home page</a>.
</p>

<p>
Please send FSF &amp; GNU inquiries to 
<a href="mailto:gnu@gnu.org"><em>gnu@gnu.org</em></a>.
There are also <a href="/home.html#ContactInfo">other ways to contact</a> 
the FSF.
<br />
Please send broken links and other corrections (or suggestions) to
<a href="mailto:webmasters@gnu.org"><em>webmasters@gnu.org</em></a>.
</p>

<p>
Copyright (C) 2004 Free Software Foundation, Inc.,
59 Temple Place - Suite 330, Boston, MA 02111, USA
<br />
Verbatim copying and distribution of this entire article is
permitted in any medium, provided this notice is preserved.
</p>

<p>
Updated:
<!-- timestamp start -->
$Date: 2004/10/13 03:13:02 $ $Author: bkorb $
<!-- timestamp end -->
</p>
</div>

</body>
</html>
