for file in ${1}
do
	echo "${file}"
    #tar -xzf ${file}
    ${2} ${file}
done

