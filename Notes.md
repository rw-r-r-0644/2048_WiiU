# 笔记
## 原项目的Reverse Engineering
项目地址：[https://github.com/gabrielecirulli/2048](https://github.com/gabrielecirulli/2048)

### CSS3动画
* [阮一峰的教程](http://www.ruanyifeng.com/blog/2014/02/css_transition_and_animation.html)
* [TimingFunction的官方定义](https://www.w3.org/TR/2012/WD-css3-transitions-20120403/#transition-timing-function)
* WebKit对cubic-bezier TimingFunction的实现：
	* [class UnitBezier](https://github.com/WebKit/webkit/blob/67985c34ffc405f69995e8a35f9c38618625c403/Source/WebCore/platform/graphics/UnitBezier.h)
	* [solveCubicBezierFunction()](https://github.com/WebKit/webkit/blob/9eec3ca633a72d2cf629895a68f4afda1bdcd47c/Source/WebCore/page/animation/AnimationBase.cpp#L58)
* [SASS中颜色混合函数（mix）的实现](https://github.com/sass/sass/blob/4415d51997bf6c2da7f0b01d2dd9d5925ef9724d/lib/sass/script/functions.rb#L1291)
* [SASS颜色函数的教程](http://www.w3cplus.com/preprocessor/sass-color-function.html)

## 此项目
* [WebKit编码规范](https://webkit.org/code-style-guidelines/)
* [Lazy Foo' Productions - Color Keying](http://lazyfoo.net/tutorials/SDL/10_color_keying/index.php) 此文讲解了Texture类的创建
* [Lazy Foo' Productions - Render to Texture](http://lazyfoo.net/tutorials/SDL/43_render_to_texture/index.php)
* [Why it is OK to return vector from function? - Stack Overflow](http://stackoverflow.com/questions/22655059/why-it-is-ok-to-return-vector-from-function) C++可以把局部vector的内容move至返回值赋给的vector，避免copy
* [stl - How do I reverse a C++ vector? - Stack Overflow](http://stackoverflow.com/questions/8877448/how-do-i-reverse-a-c-vector)
* [Explicit Return Type of Lambda - Stack Overflow](http://stackoverflow.com/questions/9620098/explicit-return-type-of-lambda) 当lambda表达式的语句数>1时，要在lambda参数表后面加入箭头来明确指定返回值类型：<pre>
\[\] (...) -> returnType { ... }
</pre>
* [STL remove doesn't work as expected?](http://stackoverflow.com/questions/6456870/stl-remove-doesnt-work-as-expected) std::remove()和std::remove_if()并不会收缩容器，要实现完整的移除还需要再调用容器的erase方法：<pre>
auto newEnd = std::remove\_if(v.begin(), v.end(), &lt;pred&gt;);
v.erase(newEnd, v.end());
</pre>

### 分析
* __游戏对象__指能渲染到屏幕上的实体。
* 虽然违反了OO设计的*单一职责原则*，但我发现把游戏对象的数据、它的绘制（render）和更新（update）放进同一个类是很自然方便的选择。
* 游戏对象可分为两类：
	* 一类是静态对象：（如TileBoard和ScoreBoard）
		* 作为容器，有子结构
		* 子Texture/子对象的渲染由该类对象代理
		* 它们有自己相对窗口的位置，运行时，位置不改变
	* 另一类是动态对象：（如Tile和ScoreAddition）
		* 用于动画
		* 通常只包含一个Texture
		* 它们的位置是相对于给定原点的，其位置由自身控制，不暴露给外部，渲染此类对象时要指定原点
