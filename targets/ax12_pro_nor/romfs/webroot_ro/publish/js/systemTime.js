(window.webpackJsonp=window.webpackJsonp||[]).push([["systemTime"],{"1e43":function(e,t,m){"use strict";m("7007")},7007:function(e,t,m){},"978a":function(e,t,m){"use strict";m.r(t);var a=m("d2db"),s=m("7187"),i=m("7886"),r=m("710d"),o={mixins:[a.a],props:{},data:()=>({timeTypeOptions:Object(r.e)(s.h),timeZoneOptions:Object(r.e)(s.f),timeOption:Object(r.e)(s.g),monthOption:Object(r.e)(s.c),weekdaysOption:Object(r.e)(i.b),whichWeekOptions:Object(r.e)(s.i),rules:{},currentTime:"",timer:null,saveTimer:null}),computed:{isManual(){return"manual"==this.formData.timeType},startYear(){return this.formData.startSummerTime.year},endYear(){return+this.formData.endSummerTime.month<=+this.formData.startSummerTime.month?+this.formData.startSummerTime.year+1:this.formData.startSummerTime.year},syncStatus(){return this.formData.isSyncInternetTime?_("Synced"):_("Unsynced")},runStatus(){return"on"==this.formData.summerTimeStatus?_("DST in use"):_("DST not use")}},mounted(){this.updateCurrentTime(),this.setTimer()},methods:{beforeSubmit(e){let t=this.formData.startSummerTime,m=this.formData.endSummerTime;return e.startSummerTime=`${this.startYear}-${t.month}-${t.whichWeek}-${t.weekdays} ${t.hours}`,e.endSummerTime=`${this.endYear}-${m.month}-${m.whichWeek}-${m.weekdays} ${m.hours}`,e.time=this.currentTime,!this.saveTimer&&(this.saveTimer=setTimeout(()=>{clearTimeout(this.saveTimer),this.saveTimer=null},2e3),e)},updateCurrentTime(){let e=new Date;this.currentTime=`${e.getFullYear()}-${e.getMonth()+1}-${e.getDate()} ${e.toTimeString().slice(0,8)}`},setTimer(){this.timer=setInterval(this.updateCurrentTime,1e3)}},destroyed(){clearInterval(this.timer),clearInterval(this.saveTimer),this.saveTimer=null}},n=(m("1e43"),m("0b56")),u={components:{timeZone:Object(n.a)(o,(function render(){var e=this,t=e._self._c;return t("v-form",{ref:e.name,attrs:{rules:e.rules}},[t("v-form-item",{attrs:{label:e._("System Time")}},[e._v(" "+e._s(e.formData.time)+" ")]),t("v-form-item",{attrs:{label:e._("Sync Status")}},[e._v(" "+e._s(e.syncStatus)+" ")]),t("v-form-item",{attrs:{label:e._("Sync Mode"),prop:"timeType"}},[t("v-select",{attrs:{options:e.timeTypeOptions},model:{value:e.formData.timeType,callback:function(t){e.$set(e.formData,"timeType",t)},expression:"formData.timeType"}})],1),e.isManual?t("v-form-item",{key:"currentTime",attrs:{label:e._("Local Time")}},[e._v(" "+e._s(e.currentTime)+" ")]):e._e(),e.isManual?e._e():t("v-form-item",{attrs:{label:e._("Time Zone"),prop:"timeZone"}},[t("v-select",{attrs:{options:e.timeZoneOptions,width:"358px"},model:{value:e.formData.timeZone,callback:function(t){e.$set(e.formData,"timeZone",t)},expression:"formData.timeZone"}})],1),t("v-form-item",{attrs:{label:e._("DST"),prop:"summerTimeEn"}},[t("v-switch",{model:{value:e.formData.summerTimeEn,callback:function(t){e.$set(e.formData,"summerTimeEn",t)},expression:"formData.summerTimeEn"}})],1),t("collapse-transition",[e.formData.summerTimeEn?t("div",[t("v-form-item",{attrs:{label:e._("Start")+" "+e.startYear}},[t("div",{staticClass:"summer-warp"},[t("v-select",{attrs:{options:e.monthOption,width:"84px"},model:{value:e.formData.startSummerTime.month,callback:function(t){e.$set(e.formData.startSummerTime,"month",t)},expression:"formData.startSummerTime.month"}}),t("v-select",{attrs:{options:e.whichWeekOptions,width:"84px"},model:{value:e.formData.startSummerTime.whichWeek,callback:function(t){e.$set(e.formData.startSummerTime,"whichWeek",t)},expression:"formData.startSummerTime.whichWeek"}})],1),t("div",{staticClass:"summer-warp"},[t("v-select",{attrs:{options:e.weekdaysOption,width:"84px"},model:{value:e.formData.startSummerTime.weekdays,callback:function(t){e.$set(e.formData.startSummerTime,"weekdays",t)},expression:"formData.startSummerTime.weekdays"}}),t("v-select",{attrs:{options:e.timeOption,width:"84px"},model:{value:e.formData.startSummerTime.hours,callback:function(t){e.$set(e.formData.startSummerTime,"hours",t)},expression:"formData.startSummerTime.hours"}})],1)]),t("v-form-item",{attrs:{label:e._("End")+" "+e.endYear}},[t("div",{staticClass:"summer-warp"},[t("v-select",{attrs:{options:e.monthOption,width:"84px"},model:{value:e.formData.endSummerTime.month,callback:function(t){e.$set(e.formData.endSummerTime,"month",t)},expression:"formData.endSummerTime.month"}}),t("v-select",{attrs:{options:e.whichWeekOptions,width:"84px"},model:{value:e.formData.endSummerTime.whichWeek,callback:function(t){e.$set(e.formData.endSummerTime,"whichWeek",t)},expression:"formData.endSummerTime.whichWeek"}})],1),t("div",{staticClass:"summer-warp"},[t("v-select",{attrs:{options:e.weekdaysOption,width:"84px"},model:{value:e.formData.endSummerTime.weekdays,callback:function(t){e.$set(e.formData.endSummerTime,"weekdays",t)},expression:"formData.endSummerTime.weekdays"}}),t("v-select",{attrs:{options:e.timeOption,width:"84px"},model:{value:e.formData.endSummerTime.hours,callback:function(t){e.$set(e.formData.endSummerTime,"hours",t)},expression:"formData.endSummerTime.hours"}})],1)]),t("v-form-item",{attrs:{label:e._("Status")}},[e._v(" "+e._s(e.runStatus)+" ")])],1):e._e()])],1)}),[],!1,null,null,null).exports},data:()=>({pageTitle:_("System Time"),pageTips:_("Functions such as Parental Control, Smart Power Saving and Auto System Maintenance are all invlove time. To make sure they take effect properly, you are recommended to select Sync with internet time."),isloading:!0,systemTime:{},timer:null}),created(){this.updateData(),this.setTimer()},methods:{init(){this.updateData()},updateData(){this.$getData({modules:"systemTime"}).then(e=>{e.systemTime.startSummerTime=this.formatTime(e.systemTime.startSummerTime),e.systemTime.endSummerTime=this.formatTime(e.systemTime.endSummerTime),this.systemTime=Object(r.e)(e.systemTime),this.isloading=!1})},updateTime(){this.$getData({modules:"systemTime"}).then(e=>{this.systemTime.time=e.systemTime.time,this.systemTime.summerTimeStatus=e.systemTime.summerTimeStatus})},setTimer(){this.timer=setInterval(this.updateTime,5e3)},formatTime(e){let t=/(?<year>.{0,})-(?<month>.{0,})-(?<whichWeek>.{0,})-(?<weekdays>.{0,}) (?<hours>.{0,})/;if(e){let m=e.match(t);return{year:m[1],month:m[2],whichWeek:m[3],weekdays:m[4],hours:m[5]}}return{year:(new Date).getFullYear(),month:1,whichWeek:1,weekdays:1,hours:"00:00"}}},beforeDestroy(){clearInterval(this.timer)}},l=Object(n.a)(u,(function render(){var e=this._self._c;return e("div",{staticClass:"firewall-set"},[e("v-page-title",{attrs:{title:this.pageTitle,tips:this.pageTips}}),e("v-loading",{attrs:{visible:this.isloading}}),this.isloading?this._e():e("v-page",{ref:"page",on:{init:this.init}},[e("time-zone",{attrs:{name:"systemTime",formData:this.systemTime}})],1)],1)}),[],!1,null,null,null);t.default=l.exports}}]);