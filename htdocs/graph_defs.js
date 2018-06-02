ccchart.base('', {config : {
    "type" : "line", //チャート種類
    "useVal" : "yes", //値を表示
    "xScaleFont" : "100 9px 'meiryo'", //水平軸目盛フォント
    "yScaleFont" : "100 12px 'meiryo'", //垂直軸目盛フォント
    "hanreiFont" : "bold 9px 'meiryo'", //凡例フォント
    "valFont" : "bold 9px 'meiryo'", //値フォント
//    "paddingTop" : "25", //上パディング
//    "paddingRight" : "140", //右パディング
    "colorSet" : ["blue"], //データ列の色
    "useShadow" : "no", //影
    "height" : "300", //チャート高さ
    "width" : "900", //チャート幅
    "useMarker" : "arc", //マーカー種類
    "markerWidth" : "5", //マーカー大きさ
    "valYOffset" : "8", //値オフセット
    "valXOffset" : "-8", //値オフセット
    "bg" : "#fff", //背景色
    "textColor" : "#333", //テキスト色
    "lineWidth" : "1", //ラインの太さ
//    "onlyChartWidthTitle": "yes",
    "useHanrei": "no",
}});

var data_temperature = {

  "config": {
    "title": "温度",
//    "subTitle": "Canvasを使った普通な感じのベジェラインチャートです",
//    "xScaleXOffset": 4,
//    "minY": 0,
//    "maxY": 100,
    "axisYLen": 10,
    "colorSet": ["#FF6666"],
  },

  "data": "./cgi-bin/get_log.cgi?mode=temperature"
};

var data_humidity = {

  "config": {
    "title": "湿度",
//    "subTitle": "Canvasを使った普通な感じのベジェラインチャートです",
//    "xScaleXOffset": 4,
//    "minY": 0,
//    "maxY": 100,
    "axisYLen": 10,
    "colorSet": ["#3333CC"],
  },

  "data": "./cgi-bin/get_log.cgi?mode=humidity"
};

var data_pressure = {

  "config": {
    "title": "気圧",
//    "subTitle": "Canvasを使った普通な感じのベジェラインチャートです",
//    "xScaleXOffset": 4,
//    "minY": 0,
//    "maxY": 100,
    "axisYLen": 10,
    "colorSet": ["#CC9933"],
  },

  "data": "./cgi-bin/get_log.cgi?mode=pressure"
};
