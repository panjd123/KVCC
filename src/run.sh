GRAPH=(dblp google stanford)
K_DBLP=(40 35 30 25 20)
K_GOOGLE=(40 35 30 25)
K_STANFORD=(40)

# MAIN="./kvcc_baseline"
# RESULT_DIR="../results"

MAIN="./kvcc_sweep"
RESULT_DIR="../results_sweep"

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