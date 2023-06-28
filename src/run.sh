# GRAPH=(dblp google stanford)
GRAPH=(dblp)
K_DBLP=(20 25 30 35 40)
K_GOOGLE=(25 30 35 40)
K_STANFORD=(40)

MAIN="./kvcc_baseline"
RESULT_DIR="../results"
if [ ! -d $RESULT_DIR ]; then
    mkdir $RESULT_DIR
fi

for g in ${GRAPH[@]}; do
    if [ $g == "dblp" ]; then
        for k in ${K_DBLP[@]}; do
            outputFileName="$RESULT_DIR/$g-k$k.txt"
            echo "Running $g with k=$k > $outputFileName"
            $MAIN -g $g -k $k -o $outputFileName
        done
    elif [ $g == "google" ]; then
        for k in ${K_GOOGLE[@]}; do
            outputFileName="$RESULT_DIR/$g-k$k.txt"
            echo "Running $g with k=$k > $outputFileName"
            $MAIN -g $g -k $k -o $outputFileName
        done
    elif [ $g == "stanford" ]; then
        for k in ${K_STANFORD[@]}; do
            outputFileName="$RESULT_DIR/$g-k$k.txt"
            echo "Running $g with k=$k > $outputFileName"
            $MAIN -g $g -k $k -o $outputFileName
        done
    fi
done