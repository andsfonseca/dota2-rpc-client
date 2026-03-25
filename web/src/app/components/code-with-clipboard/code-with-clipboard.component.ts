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

  @Input({ required: true }) text!: string;

  copied = false;

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
