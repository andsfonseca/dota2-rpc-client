import { AfterViewInit, Component, ElementRef, HostListener, ViewChild } from '@angular/core';

@Component({
  selector: 'app-root',
  templateUrl: './index.component.html',
  styleUrls: ['./index.component.scss'],
  host: {
    id: "wrapper"
  }
})

export class IndexComponent implements AfterViewInit {

  @ViewChild('intro') private introElement!: ElementRef;
  @ViewChild('next') private nextElement!: ElementRef;

  private introContentDOM: any;

  private lastPagePosition: number = -1;
  private isScrolling: boolean = false;
  private isScrollingToTop: boolean = false;
  private pageSize: number = 0;

  private scrollingTimeout!: any;

  detectIntro() {
    let pos = window.scrollY;

    if (this.isScrolling) {
      this.lastPagePosition = pos;
      return;
    }
    
    //If has scrolling
    if (!(this.lastPagePosition < 0)) {
      if (pos < this.pageSize) {

        if (this.lastPagePosition - pos < 0) {
          this.nextElement.nativeElement.scrollIntoView({ behavior: "smooth", block: "start", inline: "nearest" });
          this.isScrollingToTop = false;
        }
        else {
          this.introElement.nativeElement.scrollIntoView({ behavior: "smooth", block: "start", inline: "nearest" });
          this.isScrollingToTop = true;
        }
        this.isScrolling = true;
      }
    }

    this.lastPagePosition = pos;
  }

  scrollLoopEvent() {
    clearTimeout(this.scrollingTimeout);
    this.scrollingTimeout = setTimeout(() => {
      this.onScrollEnd();
    }, 100);
  }

  updateIntroClass() {
    let value = window.scrollY / this.pageSize;
    let nodeList: NodeListOf<any> = document.querySelectorAll('.content')
    for (let i = 0, len = nodeList.length; i < len; i++)
      nodeList[i].style.backgroundColor = 'rgba(255, 255, 255, ' + value + ')'
  }

  @HostListener('window:scroll')
  onScrollChange() {
    this.scrollLoopEvent();
    this.detectIntro();
    this.updateIntroClass();
  }

  onScrollEnd() {

    let pos = window.scrollY;

    //Stay on Top
    if (0 < pos && pos < this.pageSize) {
      if (this.isScrollingToTop)
        this.introElement.nativeElement.scrollIntoView({ behavior: "smooth", block: "start", inline: "nearest" });
      else
        this.nextElement.nativeElement.scrollIntoView({ behavior: "smooth", block: "start", inline: "nearest" });
      this.isScrolling = true;
    }
    else {
      this.isScrolling = false;
    }
  }

  @HostListener('window:resize')
  onResize() {
    this.pageSize = window.innerHeight
  }

  public onClickDown(){
    this.nextElement.nativeElement.scrollIntoView({ behavior: "smooth", block: "start", inline: "nearest" });
  }

  ngAfterViewInit() {
    this.introContentDOM = this.introElement.nativeElement.querySelector('.content')

    this.onResize();
    this.onScrollChange();
  }

}

// $(window).scroll(function() {    
//   var scroll = $(window).scrollTop();

//    //>=, not <=
//   if (scroll >= 500) {
//       //clearHeader, not clearheader - caps H
//       $(".clearHeader").addClass("darkHeader");
//   }
// });