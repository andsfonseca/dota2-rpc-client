import { Component, Input } from '@angular/core';
import { HighlightModule } from 'ngx-highlightjs';

@Component({
  selector: 'code-with-clipboard',
  templateUrl: './code-with-clipboard.component.html',
  styleUrls: ['./code-with-clipboard.component.scss'],
  imports: [HighlightModule],
  standalone: true
})
export class CodeWithClipboardComponent {

  /**
   * The code to be displayed and copied to the clipboard. This input is required and should be a string.
   */
  @Input({ required: true }) text!: string;
  /**
   * A flag indicating whether the text has been copied to the clipboard.
   */
  copied = false;

  /**
   * An method that copies the provided text to the clipboard using the Clipboard API. If the text 
   * has already been copied, it does nothing. If the copy operation is successful, it sets the 
   * `copied` flag to true for 1.5 seconds before resetting it to false. If an error occurs during 
   * the copy operation, it logs the error to the console.
   * @returns Promise that resolves when the copy operation is complete.
   */
  async copy(): Promise<void> {

    if (this.copied)
      return;

    try {
      await navigator.clipboard.writeText(this.text);
      this.copied = true;

      setTimeout(() => {
        this.copied = false;
      }, 1500);

    } catch (err) {
      console.error('Erro ao copiar:', err);
    }
  }
}
