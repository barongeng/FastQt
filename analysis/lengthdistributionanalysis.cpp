#include "lengthdistributionanalysis.h"
#include "sequence.hxx"
LengthDistributionAnalysis::LengthDistributionAnalysis()
{
    setName("Sequence Length Distribution");
    setDescription("Shows the distribution of sequence length over all sequences");
}

void LengthDistributionAnalysis::processSequence(const Sequence &sequence)
{

    int seqLen = sequence.size();
    if (seqLen + 2 > mLengthCounts.length())
        mLengthCounts.resize(seqLen + 2);

    ++mLengthCounts[seqLen];

}

void LengthDistributionAnalysis::reset()
{
    mLengthCounts.clear();
}

QWidget *LengthDistributionAnalysis::createResultWidget()
{

    computeDistribution();

    QChartView * view = new QChartView;

    QLineSeries * serie = new QLineSeries;

    for (int i=0; i<mGraphCounts.length(); ++i)
        serie->append(i, mGraphCounts[i]);

    QChart * chart = new QChart ;
    chart->addSeries(serie);
    chart->createDefaultAxes();
    chart->setTitle("Quality per base");
    chart->setAnimationOptions(QChart::NoAnimation);

    view->setChart(chart);

    return view;}

void LengthDistributionAnalysis::computeDistribution()
{
    int maxLen = 0;
    int minLen = -1;
    mMax = 0;

    qDebug()<<mLengthCounts.length();
    // Find the min and max lengths
    for (int i=0;i<mLengthCounts.length();i++) {
        if (mLengthCounts[i]>0) {
            if (minLen < 0) {
                minLen = i;
            }
            maxLen = i;
        }
    }

    // We put one extra category either side of the actual size
    if (minLen>0) minLen--;
    maxLen++;

    QVector<int> startAndInterval = sizeDistribution(minLen, maxLen);

    // Work out how many categories we need
    int categories = 0;
    int currentValue = startAndInterval[0];
    while (currentValue<= maxLen) {
        ++categories;
        currentValue+= startAndInterval[1];
    }

    mGraphCounts.resize(categories);
    mXCategories.resize(categories);

    for (int i=0;i<mGraphCounts.length();i++) {

        int minValue = startAndInterval[0]+(startAndInterval[1]*i);
        int maxValue = (startAndInterval[0]+(startAndInterval[1]*(i+1)))-1;

        if (maxValue > maxLen) {
            maxValue = maxLen;
        }

        for (int bp=minValue;bp<=maxValue;bp++) {
            if (bp < mLengthCounts.length()) {
                mGraphCounts[i] += mLengthCounts[bp];
            }
        }

        if (startAndInterval[1] == 1) {
            mXCategories[i] = ""+minValue;
        }
        else {
            mXCategories[i] = minValue+"-"+maxValue;
        }

        if (mGraphCounts[i] > mMax) mMax = mGraphCounts[i];
    }
}

QVector<int> LengthDistributionAnalysis::sizeDistribution(int min, int max)
{
    int base = 1;
    while (base > (max-min)) {
        base /= 10;
    }

    int interval =0;
    int starting =0;

    QVector<int> divisions ={1,2,5};

    // Little hack to exit the while loop..
    bool loop = true;
    while (loop) {
        for (int d=0;d<divisions.length();d++) {
            int tester = base * divisions[d];
            if (((max-min) / tester) <= 50) {
                interval = tester;
                loop = false;
                break;
            }
        }
        if (loop == true)
            base *=10;

    }

    //    // Now we work out the first value to be plotted
    int basicDivision = (int)(min/interval);

    int testStart = basicDivision * interval;

    starting = testStart;

    return {starting,interval};

}
