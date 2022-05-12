import { Component } from '@angular/core';

@Component({
  selector: 'app-root',
  templateUrl: './index.component.html',
  host:{
    id : "wrapper"
  }
})

export class IndexComponent {
  title = 'dota2-rpc-client';
}
