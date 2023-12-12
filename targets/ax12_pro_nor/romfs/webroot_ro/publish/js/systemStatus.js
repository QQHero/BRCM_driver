(window.webpackJsonp=window.webpackJsonp||[]).push([["systemStatus"],{"167a":function(t,e,i){"use strict";i("828e")},3302:function(t,e,i){},"828e":function(t,e,i){},d176:function(t,e,i){"use strict";i.r(e);var s={props:{formList:Array,title:String},methods:{isArray:t=>Array.isArray(t)}},a=(i("d255"),i("0b56")),r=Object(a.a)(s,(function render(){var t=this,e=t._self._c;return e("div",{staticClass:"form-text"},[t.title?e("v-sub-title",[t._v(t._s(t.title))]):t._e(),t._t("title"),t._l(t.formList,(function(i){return e("div",{key:i.prop,staticClass:"form-text-group",attrs:{"data-name":i.prop}},[e("div",{staticClass:"form-text__label"},[t._v(t._s(i.label))]),e("div",{staticClass:"form-text__content"},[t.isArray(i.value)?t._l(i.value,(function(i){return e("div",{key:i},[t._v(" "+t._s(i||"--")+" ")])})):[t._v(" "+t._s(i.value)+" ")]],2)])})),t._t("default")],2)}),[],!1,null,null,null).exports,o={components:{StatusList:r},props:{formData:Object},data:()=>({formList:[],title:""}),watch:{formData:{handler(t){this.formList.forEach(e=>{["wanMac","lanMac","wifiMac"].includes(e.prop)&&(t[e.prop]=t[e.prop].toUpperCase()),this.$set(e,"value",t[e.prop]||"--")})},deep:!0,immediate:!0}}},n=Object(a.a)(o,(function render(){var t=this._self._c;return t("div",[t("status-list",{attrs:{"form-list":this.formList,title:this.title}})],1)}),[],!1,null,null,null).exports,l={mixins:[n],data:()=>({title:_("Basic Info"),formList:[{label:_("Product Model"),prop:"productName"},{label:_("System Time"),prop:"systemTime"},{label:_("Runtime"),prop:"runTime"},{label:_("Firmware Version"),prop:"softVersion"},{label:_("Hardware Version"),prop:"hardVersion"}]})},p=Object(a.a)(l,void 0,void 0,!1,null,null,null).exports,u={mixins:[n],data:()=>({title:_("WAN Port Info"),formList:[{label:_("Internet Connection Status"),prop:"connectCode"},{label:_("Internet Connection Type"),prop:"wanType"},{label:_("Connected time"),prop:"connectTime"},{label:_("IP Address"),prop:"wanIP"},{label:_("Subnet Mask"),prop:"wanMask"},{label:_("Default gateway"),prop:"wanGateway"},{label:_("Primary DNS"),prop:"wanDns1"},{label:_("Secondary DNS"),prop:"wanDns2"},{label:_("MAC Address"),prop:"wanMac"}]})},d=Object(a.a)(u,void 0,void 0,!1,null,null,null).exports,f={mixins:[n],data:()=>({title:_("LAN Info"),formList:[{label:_("IP Address"),prop:"lanIP"},{label:_("Subnet Mask"),prop:"lanMask"},{label:_("MAC Address"),prop:"lanMac"}]})},c=Object(a.a)(f,void 0,void 0,!1,null,null,null).exports,m=i("710d");let w=[{label:_("Status"),prop:"wifiEn"},{label:_("Wi-Fi Name"),prop:"wifiSSID"},{label:_("Security"),prop:"wifiSecurityMode"},{label:_("Channel"),prop:"wifiChannelCurrent"},{label:_("Bandwidth"),prop:"wifiBandwidthCurrent"},{label:_("MAC Address"),prop:"wifiMac"}];var v={components:{StatusList:r},props:{formData:Object},data(){return{formList:w,formList_5g:this.getFormList("_5g"),formList_6g:this.getFormList("_6g"),CONFIG_WIFI_SUPPORT_6G:CONFIG_WIFI_SUPPORT_6G}},methods:{getFormList(t){let e=Object(m.e)(w);return e.forEach(e=>{e.prop=e.prop+t}),e}},watch:{formData:{handler(t){this.formList.forEach(e=>{this.$set(e,"value",t[e.prop]||"--")}),this.formList_5g.forEach(e=>{this.$set(e,"value",t[e.prop]||"--")}),"y"===CONFIG_WIFI_SUPPORT_6G&&this.formList_6g.forEach(e=>{this.$set(e,"value",t[e.prop]||"--")})},deep:!0,immediate:!0}}},h=(i("167a"),Object(a.a)(v,(function render(){var t=this,e=t._self._c;return e("div",[e("status-list",{attrs:{"form-list":t.formList,title:t._("")},scopedSlots:t._u([{key:"title",fn:function(){return[e("div",{staticClass:"status-sub-title"},[t._v(t._s(t._("2.4 GHz WiFi")))])]},proxy:!0}])}),e("status-list",{attrs:{"form-list":t.formList_5g},scopedSlots:t._u([{key:"title",fn:function(){return[e("div",{staticClass:"status-sub-title"},[t._v(t._s(t._("5 GHz WiFi")))])]},proxy:!0}])}),"y"===t.CONFIG_WIFI_SUPPORT_6G?e("status-list",{attrs:{"form-list":t.formList_6g},scopedSlots:t._u([{key:"title",fn:function(){return[e("div",{staticClass:"status-sub-title"},[t._v(t._s(t._("6 GHz WiFi")))])]},proxy:!0}],null,!1,2723269478)}):t._e()],1)}),[],!1,null,"007c11be",null).exports),b={mixins:[n],data:()=>({title:_("IPv6 Status"),formList:[{label:_("Connection Type"),prop:"wanType"},{label:_("IPv6 WAN Address"),prop:"wanIP"},{label:_("Default IPv6 Gateway"),prop:"wanGateway"},{label:_("Primary IPv6 DNS"),prop:"wanDns1"},{label:_("Secondary IPv6 DNS"),prop:"wanDns2"},{label:_("IPv6 LAN Address"),prop:"lanIP"}]})},S=Object(a.a)(b,void 0,void 0,!1,null,null,null).exports,g=i("05af"),y=i("539b"),I=i("5207");let C=Object(m.g)({},g.v);var L={components:{SystemList:p,WanList:d,LanList:c,WifiList:h,Ipv6List:S},data:()=>({pageTips:_("You can check the information of the router here."),systemCfg:{},wanStatus:{},wifiStatus:{},lanStatus:{},ipv6Status:{},isIpv6En:!1,isLoading:!0,updateTimer:null,isRouterMode:!1,isWispMode:!1}),created(){this.getData()},methods:{getData(){this.$getData({modules:"systemCfg,wanStatus,wifiBasicCfg,wifiAdvCfg,lanCfg,ipv6Cfg,workMode"}).then(t=>{this.isLoading=!1,this.isRouterMode="router"===t.workMode.workMode,this.isWispMode="wisp"===t.workMode.workMode,this.setSystem(t.systemCfg),(this.isRouterMode||this.isWispMode)&&(this.setWan(t.wanStatus),this.setIpv6(t.ipv6Cfg)),this.lanStatus=t.lanCfg,this.setWifi(Object(m.g)({},t.wifiBasicCfg,t.wifiAdvCfg))}),clearInterval(this.updateTimer),this.updateTimer=setInterval(this.getData,5e3)},setSystem(t){t.runTime=Object(m.h)(t.runTime),this.systemCfg=t},setWan(t){t.connectTime=Object(m.h)(t.connectTime),t.wanType=C[t.wanType],t.connectCode="1"==t.connectCode?_("N#mWf#Connected"):_("b#fLg#Disconnected"),this.wanStatus=t},setWifi(t){t.wifiEn=this.getWifiStatus(t.wifiEn,t.wifiHideSSID),t.wifiEn_5g=this.getWifiStatus(t.wifiEn_5g,t.wifiHideSSID_5g),t.wifiSecurityMode=y.v[t.wifiSecurityMode],t.wifiSecurityMode_5g=y.v[t.wifiSecurityMode_5g],"y"===CONFIG_WIFI_SUPPORT_6G&&(t.wifiEn_6g=this.getWifiStatus(t.wifiEn_5g,t.wifiHideSSID_6g),t.wifiSecurityMode_6g=y.v[t.wifiSecurityMode_6g]),this.wifiStatus=t},getWifiStatus:(t,e)=>t?e?_("Hidden"):_("Visible"):_("Disabled"),setIpv6(t){this.isIpv6En=t.ipv6En,t.wanType=I.a[t.wanType],this.ipv6Status=t}},beforeDestroy(){clearInterval(this.updateTimer)}},M=Object(a.a)(L,(function render(){var t=this,e=t._self._c;return e("div",[e("v-loading",{attrs:{visible:t.isLoading}}),t.isLoading?t._e():[e("v-page-title",{attrs:{title:t._("Router Info"),tips:t.pageTips}}),e("system-list",{attrs:{"form-data":t.systemCfg}}),t.isRouterMode||t.isWispMode?e("wan-list",{attrs:{"form-data":t.wanStatus}}):t._e(),e("lan-list",{attrs:{"form-data":t.lanStatus}}),e("wifi-list",{attrs:{"form-data":t.wifiStatus}}),t.isIpv6En&&t.isRouterMode?e("ipv6-list",{attrs:{"form-data":t.ipv6Status}}):t._e()]],2)}),[],!1,null,null,null);e.default=M.exports},d255:function(t,e,i){"use strict";i("3302")}}]);