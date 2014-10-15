for fileFig in ${1}/*.fig
do
   echo "${fileFig}"
   eval filePDF=`echo ${fileFig} | sed 's/\.fig/\.pdf/g'`
   ~/Downloads/Applications/JFig3-Launcher.app/Contents/Resources/fig2dev -L pdf -p 0 ${fileFig}  > ${filePDF}
done


