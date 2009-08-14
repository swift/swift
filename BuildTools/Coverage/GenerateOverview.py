#!/usr/bin/env python

import sys, os.path

assert(len(sys.argv) == 4)

resultsDir = sys.argv[1]
summaryFile = sys.argv[2]
overviewFile = sys.argv[3]

results = []
for dir in os.listdir(resultsDir) :
  summary = os.path.join(resultsDir, dir, summaryFile)
  if os.path.exists(summary) :
    file = open(summary)
    lines = file.readlines()
    if len(lines) == 0 or len(lines[0].split("/")) != 2 :
      continue
    coveredCount = int(lines[0].split("/")[0])
    totalCount = int(lines[0].split("/")[1])
    results.append((dir,coveredCount,totalCount))

# Compute coverage chart URL
chartparams = ["chs=320x240", "cht=lc", "chtt=Coverage (Relative)", "chxt=y", "chxl=0:|50%|80%|100%|", "chxp=0,50,80,100"]
chartdata = []
for (url,covered,total) in results :
  chartdata.append(str(100*covered/total))
chartparams.append("chd=t:" + ",".join(chartdata))
coverageChartURL = "http://chart.apis.google.com/chart?" + '&'.join(chartparams)

# Compute the maximum of lines over time
maximumNumberOfLines = 0
for (url,covered,total) in results :
  maximumNumberOfLines = max(maximumNumberOfLines,total)

# Compute code chart URL
chartparams = ["chs=320x240", "cht=lc", "chtt=Coverage (Absolute)", "chxt=y", "chxl=0:|" + str(maximumNumberOfLines) + "|", "chxp=0,100", "chm=b,FF0000,0,1,0|b,80C65A,1,2,0", "chco=00000000,00000000,00000000"]
coveredLinesData = []
totalLinesData = []
nullLinesData = []
for (url,covered,total) in results :
  coveredLinesData.append(str(100*covered/maximumNumberOfLines))
  totalLinesData.append(str(100*total/maximumNumberOfLines))
  nullLinesData.append("0")
chartparams.append("chd=t:" + ",".join(totalLinesData) + "|" + ",".join(coveredLinesData) + "|" + ",".join(nullLinesData))
codeChartURL = "http://chart.apis.google.com/chart?" + '&'.join(chartparams)


# Output page
output = open(os.path.join(resultsDir,overviewFile), 'w')
output.write("<img src=\"%(url)s\"s/>" % {'url' : coverageChartURL})
output.write("<img src=\"%(url)s\"s/>" % {'url' : codeChartURL})
output.write("<ul>\n")
for (url,covered,total) in results :
  output.write("<li><a href='%(url)s/index.html'>%(url)s</a> %(percentage)s%% (%(covered)s/%(total)s)</li>\n" % {
      'url' : url, 
      'percentage' : 100*covered / total, 
      'covered' : covered, 
      'total' : total 
    })
output.write("</ul>")
output.close()

