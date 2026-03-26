import {
  AfterViewInit,
  Component,
  ElementRef,
  HostListener,
  ViewChild,
} from "@angular/core";
import { CommonModule } from "@angular/common";
import { CodeWithClipboardComponent } from "src/app/components/code-with-clipboard/code-with-clipboard.component";

@Component({
  selector: "app-root",
  templateUrl: "./index.component.html",
  styleUrls: ["./index.component.scss"],
  host: {
    id: "wrapper",
  },
  standalone: true,
  imports: [CommonModule, CodeWithClipboardComponent],
})
export class IndexComponent implements AfterViewInit {
  /**
   * Reference to the intro section of the page
   */
  @ViewChild("intro") private introElement!: ElementRef;
  /**
   * Reference to the next section of the page
   */
  @ViewChild("next") private nextElement!: ElementRef;
  /**
   * Reference to the install section of the page
   */
  @ViewChild("install") private installElement!: ElementRef;
  /**
   * A flag indicating whether the download button was clicked, used to prevent the scroll event 
   * from being triggered when the download button is clicked
   */
  private downloadButtonWasClicked: boolean = false;
  /**
   * The last position of the page, used to detect the scroll direction
   */
  private lastPagePosition: number = -1;
  /**
   * A flag indicating whether the page is currently scrolling
   */
  private isScrolling: boolean = false;
  /**
   * A flag indicating whether the page is currently scrolling to the top
   */
  private isScrollingToTop: boolean = false;
  /**
   * The height of the page
   */
  private pageSize: number = 0;
  /**
   * A timeout used to detect when the page has stopped scrolling
   */
  private scrollingTimeout!: any;
  /**
   * The currently active download tab
   */
  activeTab: string = "windows";

  /**
   * Sets the active download tab
   * @param tab The tab to set as active, can be "windows" or "linux". If an invalid value is 
   * provided, it defaults to "windows".
   */
  setActiveTab(tab: string = "windows"): void {
    switch (tab) {
      case "windows":
      case "linux":
        this.activeTab = tab;
        break;
      default:
        this.activeTab = "windows";
    }
  }

  /**
   * Detects the user's operating system based on the user agent data. If the platform information 
   * is not available, it defaults to "windows".
   * @returns The detected operating system.
   */
  detectOS(): string {
    const platform = (navigator as any).userAgentData?.platform;
    let detectedPlatform: string = platform || "windows";
    return detectedPlatform.toLowerCase();
  }

  /**
   * Detects the intro section based on the scroll position.
   */
  detectIntro(): void {
    let pos = window.scrollY;

    if (this.isScrolling || window.innerWidth <= 1152) {
      this.lastPagePosition = pos;
      return;
    }

    //If has scrolling
    if (!(this.lastPagePosition < 0)) {
      if (pos < this.pageSize) {
        if (this.lastPagePosition - pos < 0) {
          if (this.downloadButtonWasClicked) {
            this.scrollToElement("install");
          }
          else {
            this.scrollToElement("next");
          }
          this.isScrollingToTop = false;
        } else {
          this.scrollToElement("intro");
          this.isScrollingToTop = true;
        }
        this.isScrolling = true;
      }
    }

    this.lastPagePosition = pos;
  }

  /**
   * A loop event that is triggered on scroll, it clears the previous timeout and sets a new one to
   * detect when the page has stopped scrolling.
   */
  scrollLoopEvent(): void {
    clearTimeout(this.scrollingTimeout);
    this.scrollingTimeout = setTimeout(() => {
      this.onScrollEnd();
    }, 100);
  }

  /**
   * Updates the class of the intro section based on the scroll position.
   */
  updateIntroClass() {
    if (window.innerWidth <= 1152) {
      this.isScrolling = false;
      return;
    }
    let value = window.scrollY / this.pageSize;
    let nodeList: NodeListOf<any> = document.querySelectorAll(
      "#wrapper > section > .content",
    );
    for (let i = 0, len = nodeList.length; i < len; i++)
      nodeList[i].style.backgroundColor = "rgba(255, 255, 255, " + value + ")";
  }

  /**
   * Event listener for the scroll event.
   */
  @HostListener("window:scroll")
  onScrollChange(): void {
    this.scrollLoopEvent();
    this.detectIntro();
    this.updateIntroClass();
  }

  /**
   * Event listener for when scrolling ends.
   */
  onScrollEnd(): void {
    let pos = window.scrollY;

    this.downloadButtonWasClicked = false;

    if (window.innerWidth <= 1152) {
      this.isScrolling = false;
      return;
    }

    //Stay on Top
    if (0 < pos && pos < this.pageSize) {
      if (this.isScrollingToTop)
        this.introElement.nativeElement.scrollIntoView({
          behavior: "smooth",
          block: "start",
          inline: "nearest",
        });
      else
        this.nextElement.nativeElement.scrollIntoView({
          behavior: "smooth",
          block: "start",
          inline: "nearest",
        });
      this.isScrolling = true;
    } else {
      this.isScrolling = false;
    }
  }

  /**
   * Event listener for the resize event, it updates the page size based on the new window height.
   */
  @HostListener("window:resize")
  onResize(): void {
    this.pageSize = window.innerHeight;
  }

  /**
   * Event listener for the click event on the download button, it sets the 
   * downloadButtonWasClicked flag to true to prevent the scroll event from being triggered when
   * the download button is clicked.
   */
  onClickDownloadButton(): void {
    this.downloadButtonWasClicked = true;
  }

  /**
   * Event listener for the click event on the down button, it scrolls to the next section of the page.
   */
  public scrollToElement(elementId: string): void {

    switch (elementId) {
      case "intro":
        this.introElement.nativeElement.scrollIntoView({
          behavior: "smooth",
          block: "start",
          inline: "nearest",
        });
        return;
      case "install":
        this.installElement.nativeElement.scrollIntoView({
          behavior: "smooth",
          block: "start",
          inline: "nearest",
        });
        return;
      case "next":
        this.nextElement.nativeElement.scrollIntoView({
          behavior: "smooth",
          block: "start",
          inline: "nearest",
        });
        return;
    }

  }

  /**
   * Executed after the view has been initialized.
   */
  ngAfterViewInit(): void {
    this.setActiveTab(this.detectOS());
    this.onResize();
    this.onScrollChange();
  }
}
